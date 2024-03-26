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

#include "constants.hpp"
#include "magic_wand_model_data.hpp"
#include "output_handler.h"


#define KTENSOR_ARENA_SIZE (10 * 1024)

#ifdef __cplusplus
extern "C" {
#endif

static struct {
    const tflite::Model *model;
    tflite::MicroInterpreter *interpreter;
    TfLiteTensor *model_input;
    int input_length;
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

    for (int i = 0; i < kGestureCount; ++i) {
        self.prediction_history[i][self.prediction_history_index] = output[i];
    }

    ++self.prediction_history_index;
    if (self.prediction_history_index >= kPredictionHistoryLength) {
        self.prediction_history_index = 0;
    }

    int max_predict_index = -1;
    float max_predict_score = 0.0f;
    for (int i = 0; i < kGestureCount; i++) {
        float prediction_sum = 0.0f;
        for (int j = 0; j < kPredictionHistoryLength; ++j) {
            prediction_sum += self.prediction_history[i][j];
        }
        const float prediction_average = prediction_sum / kPredictionHistoryLength;
        if ((max_predict_index == -1) || (prediction_average > max_predict_score)) {
            max_predict_index = i;
            max_predict_score = prediction_average;
        }
    }


    if (self.prediction_suppression_count > 0) {
        --self.prediction_suppression_count;
    }

    if ((max_predict_index == kNoGesture) ||
        (max_predict_score < kDetectionThreshold) ||
        (self.prediction_suppression_count > 0)) {
        return kNoGesture;
    } else {

        self.prediction_suppression_count = kPredictionSuppressionDuration;
        return max_predict_index;
    }
}

int predictor_init(void) {

    self.model = tflite::GetModel(g_magic_wand_model_data);
    if (self.model->version() != TFLITE_SCHEMA_VERSION) {
        printk("Model provided is schema version %d not equal "
               "to supported version %d.",
               self.model->version(), TFLITE_SCHEMA_VERSION);
        return -1;
    }

    static tflite::MicroMutableOpResolver<6> micro_op_resolver = tflite::MicroMutableOpResolver<6>();
    micro_op_resolver.AddConv2D();
    micro_op_resolver.AddReshape();
    micro_op_resolver.AddMaxPool2D();
    micro_op_resolver.AddExpandDims();
    micro_op_resolver.AddFullyConnected();
    micro_op_resolver.AddSoftmax();

    static tflite::MicroInterpreter static_interpreter = tflite::MicroInterpreter(
            self.model, micro_op_resolver, self.tensor_arena, KTENSOR_ARENA_SIZE);
    self.interpreter = &static_interpreter;

    TfLiteStatus status = self.interpreter->AllocateTensors();

    self.model_input = self.interpreter->input(0);
    if (status != kTfLiteOk) {
        printk("Bad input tensor parameters in model");
        return -1;
    }

    self.input_length = self.model_input->bytes / (3 * sizeof(float));

    int setup_status = accelerometer_config();
    if (setup_status) {
        printk("Set up failed Acceleremeter\n");
        return -1;
    }
    return 0;
}

void predict_thread(void) {
    uint16_t count_sample = 0;
    k_sleep(K_MSEC(1000));
    while (true) {
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