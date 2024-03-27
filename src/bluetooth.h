//
// Created by jose on 06/03/24.
//

#ifndef TENSORFLOW_MAGIC_WAND_BLUETOOTH_H
#define TENSORFLOW_MAGIC_WAND_BLUETOOTH_H

#include <zephyr/kernel.h>
#include <stddef.h>
#include <string.h>
#include <zephyr/sys/printk.h>
#include <zephyr/settings/settings.h>

#define SERVICE_UUID BT_UUID_DECLARE_16(0xf1)

#define WHITE_CARAC BT_UUID_DECLARE_16(0xf2)

#define NOTIFY_CARAC BT_UUID_DECLARE_16(0xf3)


typedef struct
{
    uint8_t data[64];
    uint16_t len;
} data_t;


int bluetooth_ready(void);

int bluetooth_send_notify(uint8_t* data, uint16_t len);
#endif //TENSORFLOW_MAGIC_WAND_BLUETOOTH_H
