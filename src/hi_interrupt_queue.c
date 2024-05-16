/**
 * @file hi_interrupt_queue.c
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

#include "hi_defines.h"
#include "hi_interrupt_queue.h"

void hi_interrupt_queue_init(hi_interrupt_queue_t *queue)
{
#if _HI_FOR_FREERTOS
    if (queue->len > 0) {
        queue->queue = xQueueCreate(queue->len, queue->item_size);
    }
#endif
}

hi_err_t hi_interrupt_queue_send(hi_interrupt_queue_t *queue, void *item, hi_ticks_t ticks_to_wait)
{
#if _HI_FOR_FREERTOS
    if (queue->len > 0) {
        if (xQueueSend(queue->queue, item, ticks_to_wait)) {
            return HI_ERR_OK_CONST;
        }
        else {
            return HI_ERR_FAILED_CONST;
        }
    }
#endif
    return HI_ERR_FAILED_CONST;
}

hi_err_t hi_interrupt_queue_send_fromISR(hi_interrupt_queue_t *queue, void *item, halo_priority_t priority)
{
#if _HI_FOR_FREERTOS
    if (queue->len > 0) {
        //TODO: Define the priority map.
        if (xQueueSendFromISR(queue->queue, item, NULL)) {
            return HI_ERR_OK_CONST;
        }
        else {
            return HI_ERR_FAILED_CONST;
        }
    }
    else {
        return HI_ERR_FAILED_CONST;
    }
#else
    return HI_ERR_FAILED_CONST;
#endif
}

hi_err_t hi_interrupt_queue_recv(hi_interrupt_queue_t *queue, void *item, hi_ticks_t ticks_to_wait)
{
#if _HI_FOR_FREERTOS
    if (queue->len > 0) {
        if (xQueueReceive(queue->queue, item, ticks_to_wait)){
            return HI_ERR_OK_CONST;
        }
        else {
            return HI_ERR_FAILED_CONST;
        }
    }
    else {
        return HI_ERR_FAILED_CONST;
    }
#else
    return HI_ERR_FAILED_CONST;
#endif
}

void hi_interrupt_queue_deinit(hi_interrupt_queue_t *queue)
{
#if _HI_FOR_FREERTOS
    if (queue->len > 0 && queue->queue)
    {
        xQueueReset(queue->queue);
        queue->queue = NULL;
    }
#endif
}
