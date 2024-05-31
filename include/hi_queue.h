/**
 * @file hi_queue.h
 * @author Cosmade (deepskystar@outlook.com)
 * @brief 
 * @version
 * @date 2024-05-28
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

#ifndef HI_LIST_H_
#define HI_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_sys.h"
#include "hi_types.h"
#include "hi_log.h"
#include "hi_memory.h"
#include "hi_thread.h"

typedef struct
{
    hi_iter_t next;
    hi_value_t value;
}hi_queue_node_t;

typedef struct
{
    hi_iter_t head;
    hi_iter_t last;
    hi_mem_pool_t *pool;    //unit size must bigger then hi_queue_node_t.
}hi_queue_t;

#define HI_QUEUE_DEFINE(__name__, __pool__) hi_queue_t __name__ = { \
    .head = HI_ITER_NULL, \
    .last = HI_ITER_NULL, \
    .pool = __pool__,   \
}

#define HI_QUEUE_NODE_NULL (hi_queue_node_t){HI_ITER_NULL, HI_VALUE_NULL}
#define HI_QUEUE_IS_EMPTY(__queue__) ((__queue__).last == HI_ITER_NULL)

extern void hi_queue_init(hi_queue_t *queue);

extern hi_result_t hi_queue_in(hi_queue_t *queue, hi_value_t value);

extern void hi_queue_out(hi_queue_t *queue);

extern uint8_t hi_queue_empty(hi_queue_t *queue);

extern hi_value_t hi_queue_head(hi_queue_t *queue);

extern hi_value_t hi_queue_last(hi_queue_t *queue);

extern hi_iter_t hi_queue_begin(hi_queue_t *queue);

extern hi_queue_node_t hi_queue_next(hi_queue_t *queue, hi_iter_t iter);

extern void hi_queue_del_next(hi_queue_t *queue, hi_iter_t iter);

extern void hi_queue_deinit(hi_queue_t *queue);


typedef struct
{
    hi_mutex_t mutex;   //mutex for create.
    hi_queue_t unsafe;
}hi_async_queue_t;

extern void hi_async_queue_init(hi_async_queue_t *queue);

extern hi_result_t hi_async_queue_in(hi_async_queue_t *queue, hi_value_t value);

extern void hi_async_queue_out(hi_async_queue_t *queue);

extern uint8_t hi_async_queue_empty(hi_async_queue_t *queue);

extern hi_value_t hi_async_queue_head(hi_async_queue_t *queue);

extern hi_value_t hi_async_queue_last(hi_async_queue_t *queue);

extern hi_iter_t hi_async_queue_begin(hi_async_queue_t *queue);

extern hi_queue_node_t hi_async_queue_next(hi_async_queue_t *queue, hi_iter_t iter);

extern void hi_async_queue_del_next(hi_async_queue_t *queue, hi_iter_t iter);

extern void hi_async_queue_deinit(hi_async_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif /* HI_LIST_H_ */
