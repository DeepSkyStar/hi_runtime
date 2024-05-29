/**
 * @file hi_async_list.h
 * @author Cosmade (deepskystar@outlook.com)
 * @brief 
 * @version
 * @date 2024-05-27
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

#ifndef HI_ASYNC_MAP_H_
#define HI_ASYNC_MAP_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "hi_sys.h"
#include "hi_types.h"
#include "hi_task.h"
#include "hi_isr_queue.h"

typedef uint32_t hi_async_key_t;
typedef struct hi_async_list_s hi_async_list_t;

typedef void (*hi_async_set_func_f)(hi_async_list_t *list, hi_value_t value, hi_task_t task);
typedef hi_value_t (*hi_async_get_func_f)(hi_async_list_t *list, hi_task_t task);

typedef void (*hi_async_set_callback_f)(hi_async_list_t *list, hi_async_key_t key, hi_result_t result);
typedef void (*hi_async_get_callback_f)(hi_async_list_t *list, hi_async_key_t key, hi_value_t value);

typedef void (*hi_async_sub_callback_f)(hi_async_list_t *list, hi_async_key_t key, hi_value_t old, hi_value_t new);

typedef struct
{
    void* (*func)(void* args);
}hi_async_func_t;

typedef struct
{
    uint8_t type;
    hi_value_t value;
    hi_async_set_func_f setter;
    hi_async_get_func_f getter;
}hi_async_value_t;

typedef struct hi_async_list_s
{
    hi_async_value_t *container;
    uint32_t length;
    hi_isr_queue_t *task_queue;
    hi_isr_queue_t *sub_queue;
}hi_async_list_t;

extern void hi_async_list_init(hi_async_list_t *list);

extern hi_task_handle_t hi_async_list_set(
    hi_async_list_t *list, 
    hi_async_key_t key, 
    hi_value_t value, 
    hi_async_set_callback_f callback);

extern hi_task_handle_t hi_async_list_get(
    hi_async_list_t *list, 
    hi_async_key_t key, 
    hi_async_get_callback_f callback);

extern void hi_async_list_cancel(hi_async_list_t *list, hi_task_handle_t task);

extern hi_task_handle_t hi_async_list_sub(hi_async_list_t *list, hi_async_key_t key, hi_async_sub_callback_f callback);
extern void hi_async_list_unsub(hi_async_list_t *list, hi_task_handle_t handle);
extern void hi_async_list_unsub_all(hi_async_list_t *list);
extern void hi_async_list_deinit(hi_async_list_t *list);

extern void hi_async_list_accept(
    hi_async_list_t *list, 
    hi_async_key_t key, 
    hi_task_handle_t task);


#ifdef __cplusplus
}
#endif

#endif /* HI_ASYNC_MAP_H_ */
