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

#ifndef HI_QUEUE_H_
#define HI_QUEUE_H_

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
    hi_iter_t prev;
    hi_iter_t next;
    uint8_t data[0];
}hi_queue_node_t;

typedef struct
{
    hi_iter_t first;
    hi_iter_t last;
    hi_size_t usage;
    hi_mem_pool_t *pool;    //unit size must bigger then hi_queue_node_t.
}hi_queue_t;

#define HI_QUEUE_NODE_NULL (hi_queue_node_t){HI_ITER_NULL, HI_VALUE_NULL}
#define HI_QUEUE_IS_EMPTY(__queue__) ((__queue__).last == HI_ITER_NULL)

#define HI_QUEUE_POOL_DEFINE(__pool__, __data_size__, __count__) HI_MEM_POOL_DEFINE(__pool__, sizeof(hi_queue_node_t) +  __data_size__, __count__)

#define HI_QUEUE_INIT(__pool__) { \
    .first = HI_ITER_NULL, \
    .last = HI_ITER_NULL, \
    .pool = __pool__,   \
}

//If the queue is not static define, must be free at the end.
//After new, no need to call init.
extern hi_queue_t* hi_queue_new(hi_size_t data_size, hi_size_t max_size);
extern void hi_queue_init(hi_queue_t *queue);
extern void hi_queue_deinit(hi_queue_t *queue);
extern void hi_queue_free(hi_queue_t *queue);

extern hi_iter_t hi_queue_try_add(hi_queue_t *queue);

extern hi_iter_t hi_queue_add_first(hi_queue_t *queue, const void* data, hi_size_t size);
extern hi_iter_t hi_queue_add_first_value(hi_queue_t *queue, hi_value_t value);
extern hi_iter_t hi_queue_add_last(hi_queue_t *queue, const void* data, hi_size_t size);
extern hi_iter_t hi_queue_add_last_value(hi_queue_t *queue, hi_value_t value);

extern hi_iter_t hi_queue_insert_next(hi_queue_t *queue, hi_iter_t iter, const void *data, hi_size_t size);
extern hi_iter_t hi_queue_insert_next_value(hi_queue_t *queue, hi_iter_t iter, hi_value_t value);
extern hi_iter_t hi_queue_insert_prev(hi_queue_t *queue, hi_iter_t iter, const void *data, hi_size_t size);
extern hi_iter_t hi_queue_insert_prev_value(hi_queue_t *queue, hi_iter_t iter, hi_value_t value);

extern void* hi_queue_get(hi_queue_t *queue, hi_iter_t iter);
extern void* hi_queue_get_first(hi_queue_t *queue);
extern void* hi_queue_get_last(hi_queue_t *queue);

extern hi_queue_node_t* hi_queue_get_node(hi_queue_t *queue, hi_iter_t iter);
extern hi_size_t hi_queue_get_usage(hi_queue_t *queue);
extern hi_size_t hi_queue_get_data_size(hi_queue_t *queue);
extern hi_size_t hi_queue_get_max_size(hi_queue_t *queue);

extern void hi_queue_del_first(hi_queue_t *queue);
extern void hi_queue_del_last(hi_queue_t *queue);
extern void hi_queue_del(hi_queue_t *queue, hi_iter_t iter);
extern void hi_queue_del_all(hi_queue_t *queue);

extern hi_iter_t hi_queue_begin(hi_queue_t *queue);
extern hi_iter_t hi_queue_end(hi_queue_t *queue);
extern hi_iter_t hi_queue_next(hi_queue_t *queue, hi_iter_t iter);
extern hi_iter_t hi_queue_prev(hi_queue_t *queue, hi_iter_t iter);

typedef struct
{
    hi_mutex_t mutex;   //mutex for create.
    hi_queue_t unsafe;
}hi_sync_queue_t;

extern hi_sync_queue_t* hi_sync_queue_new(hi_size_t data_size, hi_size_t max_size);
extern void hi_sync_queue_init(hi_sync_queue_t *queue);
extern void hi_sync_queue_deinit(hi_sync_queue_t *queue);
extern void hi_sync_queue_free(hi_sync_queue_t *queue);

extern hi_iter_t hi_sync_queue_try_add(hi_sync_queue_t *queue);
extern hi_iter_t hi_sync_queue_add_first(hi_sync_queue_t *queue, const void* data, hi_size_t size);
extern hi_iter_t hi_sync_queue_add_first_value(hi_sync_queue_t *queue, hi_value_t value);
extern hi_iter_t hi_sync_queue_add_last(hi_sync_queue_t *queue, const void* data, hi_size_t size);
extern hi_iter_t hi_sync_queue_add_last_value(hi_sync_queue_t *queue, hi_value_t value);

extern hi_iter_t hi_sync_queue_insert_next(hi_sync_queue_t *queue, hi_iter_t iter, const void *data, hi_size_t size);
extern hi_iter_t hi_sync_queue_insert_next_value(hi_sync_queue_t *queue, hi_iter_t iter, hi_value_t value);
extern hi_iter_t hi_sync_queue_insert_prev(hi_sync_queue_t *queue, hi_iter_t iter, const void *data, hi_size_t size);
extern hi_iter_t hi_sync_queue_insert_prev_value(hi_sync_queue_t *queue, hi_iter_t iter, hi_value_t value);

extern void* hi_sync_queue_get(hi_sync_queue_t *queue, hi_iter_t iter);
extern void* hi_sync_queue_get_first(hi_sync_queue_t *queue);
extern void* hi_sync_queue_get_last(hi_sync_queue_t *queue);

extern hi_queue_node_t* hi_sync_queue_get_node(hi_sync_queue_t *queue, hi_iter_t iter);
extern hi_size_t hi_sync_queue_get_usage(hi_sync_queue_t *queue);
extern hi_size_t hi_sync_queue_get_data_size(hi_sync_queue_t *queue);
extern hi_size_t hi_sync_queue_get_max_size(hi_sync_queue_t *queue);

extern void hi_sync_queue_del_first(hi_sync_queue_t *queue);
extern void hi_sync_queue_del_last(hi_sync_queue_t *queue);
extern void hi_sync_queue_del(hi_sync_queue_t *queue, hi_iter_t iter);
extern void hi_sync_queue_del_all(hi_sync_queue_t *queue);

extern hi_iter_t hi_sync_queue_begin(hi_sync_queue_t *queue);
extern hi_iter_t hi_sync_queue_end(hi_sync_queue_t *queue);
extern hi_iter_t hi_sync_queue_next(hi_sync_queue_t *queue, hi_iter_t iter);
extern hi_iter_t hi_sync_queue_prev(hi_sync_queue_t *queue, hi_iter_t iter);

#ifdef __cplusplus
}
#endif

#endif /* HI_QUEUE_H_ */
