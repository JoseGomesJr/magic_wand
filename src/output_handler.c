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

#include "output_handler.h"
#include "bluetooth.h"

void classificatiopn_predict(uint8_t kind)
{
	if (kind == 0) {
        bluetooth_send_notify("O", 1);
        printk("O\n");
	} else if (kind == 1) {
        bluetooth_send_notify("V", 1);
        printk("V\n");
	} else if (kind == 2) {
        bluetooth_send_notify("Z", 1);
        printk("Z\n");
	}

//    bluetooth_send_notify("SIMM", 1);
}