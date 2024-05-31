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
#include "hi_memory.h"

typedef hi_iter_t hi_task_id_t;
typedef struct hi_task_pool_s hi_task_pool_t;
typedef struct hi_task_s hi_task_t;
typedef void* (*hi_task_func_t)(const hi_task_t *task, hi_result_t result, void *args);

#define HI_TASK_IS_INVALID(__task_id__) HI_ITER_IS_INVALID(__task_id__)

typedef struct
{
    uint8_t is_started:1;
    uint8_t is_cancelled:1;
    uint8_t is_finished:1;
    uint8_t reserved;
}hi_task_state_t;

struct hi_task_s
{
    hi_task_id_t task_id;   //should not be changed by self.
    hi_task_func_t func;    //would be call when task is run.
    hi_value_t owner;   //the owner of this task.
};

struct hi_task_pool_s
{
    //SHOULD NOT BE USED shared pool.
    hi_mem_pool_t pool;    //the block size must bigger that hi_task_t.
};

#define HI_TASK_POOL_INIT(__pool__, __task_size__, __max_count__) {    \
    .pool = HI_MEM_POOL_INIT(__pool__, __task_size__, __max_count__)  \
}

//add a task to pool to storage.
hi_task_id_t hi_task_pool_add(hi_task_pool_t *pool, hi_task_t task);

//after run it will remove from task immediately.
void hi_task_pool_run(hi_task_pool_t *pool, hi_task_id_t task_id, hi_result_t result, void *args);

//get the task for run multi.
hi_task_t hi_task_pool_get(hi_task_pool_t *pool, hi_task_id_t task_id);

//if task cancel, the result will be HI_RESULT_CANCELLED.
void hi_task_pool_cancel(hi_task_pool_t *pool, hi_task_id_t task_id);


#ifdef __cplusplus
}
#endif

#endif /* HI_TASK_H_ */
