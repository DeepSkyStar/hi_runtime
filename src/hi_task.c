/**
 * @file hi_task.c
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

#include "hi_task.h"

//add a task to pool to storage.
hi_task_id_t hi_task_pool_add(hi_task_pool_t *pool, hi_task_t task)
{
    return 0;
}

//after run it will remove from task immediately.
void hi_task_pool_run(hi_task_pool_t *pool, hi_task_id_t task_id, hi_result_t result, void *args)
{

}

//get the task for run multi.
hi_task_t hi_task_pool_get(hi_task_pool_t *pool, hi_task_id_t task_id)
{
    return (hi_task_t){0};
}

//if task cancel, the result will be HI_RESULT_CANCELLED.
void hi_task_pool_cancel(hi_task_pool_t *pool, hi_task_id_t task_id)
{

}