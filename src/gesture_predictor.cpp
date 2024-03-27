/*
 * Copyright 2019 The TensorFlow Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "gesture_predictor.h"

#include <tensorflow/lite/micro/micro_interpreter.h>
#include <tensorflow/lite/micro/micro_mutable_op_resolver.h>
#include <zephyr/logging/log.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>

#include "constants.hpp"
#include "magic_wand_model_data.hpp"
#include "output_handler.h"


#define SW0_NODE	DT_ALIAS(sw0)
#if !DT_NODE_HAS_STATUS(SW0_NODE, okay)
#error "Unsupported board: sw0 devicetree alias is not defined"
#endif
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios,
                                                              {0});
static struct gpio_callback button_cb_data;

#define KTENSOR_ARENA_SIZE (10 * 1024)

#ifdef __cplusplus
extern "C" {
#endif

static struct {
    const tflite::Model *model;
    tflite::MicroInterpreter *interpreter;
    TfLiteTensor *model_input;
    int input_length;
    size_t n_classes;
    TfLiteTensor* output_tensor;

    uint8_t tensor_arena[KTENSOR_ARENA_SIZE];

    float prediction_history[kGestureCount][kPredictionHistoryLength];
    int prediction_history_index;
    int prediction_suppression_count;
} self = {
        .model = nullptr,
        .interpreter = nullptr,
        .model_input = nullptr,
        .input_length = 0,

        .prediction_history = {{0}},
        .prediction_history_index = 0,
        .prediction_suppression_count = 0,
};

extern const k_tid_t predict_thread_id;

int PredictGesture(const float *output) {

    TfLiteStatus status;
    uint8_t argmax = -1;
    float max = 0.0f;

    argmax = 0;
    max = output[argmax];

    float z, o, v;

    o= output[0];
    v = output[1];
    z = output[2];
    for (uint8_t i = 0; i < self.n_classes; i++) {
        if (output[i] > max) {
            max = output[i];
            argmax = i;
        }
    }

    printk("V: %f, 0:%f, Z:%f/n", v, o, z);
    return argmax;
}

void button_pressed(const struct device *dev, struct gpio_callback *cb,
                    uint32_t pins)
{
    printk("Button pressed at %" PRIu32 "\n", k_cycle_get_32());
}

int init_button(){
    int ret = 0;
    if (!gpio_is_ready_dt(&button)) {
        printk("Error: button device %s is not ready\n",
               button.port->name);
        return -1;
    }

    ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
    if (ret != 0) {
        printk("Error %d: failed to configure %s pin %d\n",
               ret, button.port->name, button.pin);
        return -1;
    }

    ret = gpio_pin_interrupt_configure_dt(&button,
                                          GPIO_INT_EDGE_TO_ACTIVE);
    if (ret != 0) {
        printk("Error %d: failed to configure interrupt on %s pin %d\n",
               ret, button.port->name, button.pin);
        return -1;
    }

    gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
    gpio_add_callback(button.port, &button_cb_data);
    printk("Set up button at %s pin %d\n", button.port->name, button.pin);

    return ret;
}

int predictor_init(void) {

    if(init_button()){
        printk("Button não encontrado\n");
    }
    self.model = tflite::GetModel(g_magic_wand_model_data);
    if (self.model->version() != TFLITE_SCHEMA_VERSION) {
        printk("Model provided is schema version %d not equal "
               "to supported version %d.",
               self.model->version(), TFLITE_SCHEMA_VERSION);
        return -1;
    }

    static tflite::MicroMutableOpResolver<3> micro_op_resolver = tflite::MicroMutableOpResolver<3>();
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddFullyConnected();
    micro_op_resolver.AddSoftmax();

    static tflite::MicroInterpreter static_interpreter = tflite::MicroInterpreter(
            self.model, micro_op_resolver, self.tensor_arena, KTENSOR_ARENA_SIZE);
    self.interpreter = &static_interpreter;

    TfLiteStatus status = self.interpreter->AllocateTensors();

    self.model_input = self.interpreter->input(0);
    self.output_tensor = self.interpreter->output_tensor(0);
    if (status != kTfLiteOk) {
        printk("Bad input tensor parameters in model");
        return -1;
    }

    self.input_length = self.model_input->bytes / (3 * sizeof(float));
    self.n_classes = self.output_tensor->bytes / sizeof(float);

    int setup_status = accelerometer_config();
    if (setup_status) {
        printk("Set up failed Acceleremeter\n");
        return -1;
    }
    return 0;
}

void predict_thread(void) {
    uint16_t count_sample = 0;
    k_sleep(K_MSEC(5000));
    while (true) {
        int val = gpio_pin_get_dt(&button);

        if (!val) {
            count_sample = 0;
            for (int i = 0; i < 3 * self.input_length; i++) {
                self.model_input->data.f[i] = 0.0f;
            }
            k_sleep(K_MSEC(10));
            continue;
        }

        bool got_data =
                accelerometer_read(self.model_input->data.f + 3 * count_sample, self.input_length);
        if (!got_data) {
            return;
        }
        count_sample ++;
        if(count_sample < self.input_length){
            k_sleep(K_MSEC(10));
            continue;
        }

        TfLiteStatus invoke_status = self.interpreter->Invoke();
        if (invoke_status != kTfLiteOk) {
            printk("Invoke failed on index\n");
            return;
        }
        int gesture_index = PredictGesture(self.interpreter->output(0)->data.f);

        classificatiopn_predict(gesture_index);

        count_sample = 0;
        for (int i = 0; i < 3 * self.input_length; i++) {
            self.model_input->data.f[i] = 0.0f;
        }
        k_sleep(K_MSEC(10));
    }
}

K_THREAD_DEFINE(predict_thread_id, 2048,
                predict_thread, NULL, NULL, NULL, 5, 0, 1000);

#ifdef __cplusplus
};
#endif