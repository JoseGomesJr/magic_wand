#ifndef TENSORFLOW_MAGIC_WAND_ACCELEROMETER_H
#define TENSORFLOW_MAGIC_WAND_ACCELEROMETER_H

#include <zephyr/dsp/types.h>

#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <string.h>
#include <zephyr/kernel.h>

int accelerometer_config();

bool accelerometer_read(float *input, int length);

#endif //TENSORFLOW_MAGIC_WAND_ACCELEROMETER_H
