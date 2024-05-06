/**
 * @file hi_interrupt_queue.h
 * @author Cosmade (deepskystar@outlook.com)
 * @brief 
 * @version
 * @date 2024-05-06
 * 
 * @copyright Copyright 2024 Cosmade
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef HI_INTERRUPT_QUEUE_H_
#define HI_INTERRUPT_QUEUE_H_

#include "hi_defines.h"
#include "hi_error.h"
#include "hi_time.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hi_interrupt_queue_s{
    void *queue;
    uint16_t item_size;
    uint16_t len;
}hi_interrupt_queue_t;

typedef uint8_t halo_priority_t;
typedef enum{
    HALO_PRIORITY_BACKGROUND = 0,   //for data storage or something not important.
    HALO_PRIORITY_NORMAL = 1,   //usually task.
    HALO_PRIORITY_RESPONSE = 2, //for response user input.
    HALO_PRIORITY_DISPLAY = 3,  //for display sync.
    HALO_PRIORITY_CRITICAL = 4, //for critical safety function.
}halo_priority_enum;

void hi_interrupt_queue_init(hi_interrupt_queue_t *queue);
hi_err_t hi_interrupt_queue_send(hi_interrupt_queue_t *queue, void *item, hi_ticks_t ticks_to_wait);
hi_err_t hi_interrupt_queue_send_fromISR(hi_interrupt_queue_t *queue, void *item, halo_priority_t priority);
hi_err_t hi_interrupt_queue_recv(hi_interrupt_queue_t *queue, void *item, hi_ticks_t ticks_to_wait);
void hi_interrupt_queue_deinit(hi_interrupt_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif