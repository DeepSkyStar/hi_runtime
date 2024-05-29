/**
 * @file hi_isr_queue.h
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

#include "hi_sys.h"
#include "hi_types.h"
#include "hi_time.h"
#include "hi_thread.h"
#include "hi_log.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    void *queue;
    uint16_t item_size;
    uint16_t len;
}hi_isr_queue_t;

void hi_isr_queue_init(hi_isr_queue_t *queue);
hi_result_t hi_isr_queue_send(hi_isr_queue_t *queue, void *item, hi_ticks_t ticks_to_wait);
hi_result_t hi_isr_queue_send_fromISR(hi_isr_queue_t *queue, void *item, hi_priority_t priority);
hi_result_t hi_isr_queue_recv(hi_isr_queue_t *queue, void *item, hi_ticks_t ticks_to_wait);
void hi_isr_queue_deinit(hi_isr_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif