/**
 * @file hi_task.h
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

#ifndef HI_TASK_H_
#define HI_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_sys.h"
#include "hi_types.h"
#include "hi_runloop.h"

typedef uint32_t hi_task_handle_t;
typedef struct hi_task_pool_s hi_task_pool_t;
typedef void* (*hi_task_func_t)(hi_task_pool_t *pool, hi_task_handle_t handle, hi_result_t result, void *args);

typedef struct
{
    uint8_t is_started:1;
    uint8_t is_finished:1;
}hi_task_state_t;

typedef struct
{
    hi_task_handle_t handle;
    hi_task_func_t func;
}hi_task_t;

struct hi_task_pool_s
{
    void *queue;
    uint16_t item_size;
    uint16_t len;
};

void hi_task_pool_init(hi_task_pool_t *pool);
hi_task_handle_t hi_task_pool_add(hi_task_pool_t *pool, hi_task_t *task);
void hi_task_pool_call(hi_task_pool_t *pool, hi_task_handle_t handle, hi_result_t result, void *args);
void hi_task_pool_cancel(hi_task_pool_t *pool, hi_task_t *task);


#ifdef __cplusplus
}
#endif

#endif /* HI_TASK_H_ */
