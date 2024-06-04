/**
 * @file hi_thread.h
 * @author Cosmade (deepskystar@outlook.com)
 * @brief used to instead of thread.
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

#ifndef HI_THREAD_H_
#define HI_THREAD_H_

#ifdef __cplusplus
extern "C" 
#endif

#include "hi_sys.h"
#include "hi_types.h"
#include "hi_time.h"

typedef struct
{

#if _HI_PTHREAD
    pthread_mutex_t *mutex;
#elif _HI_FREERTOS
    // SemaphoreHandle_t mutex;
    void *mutex;
#else
    uint32_t mutex;
#endif
}hi_mutex_t;

typedef struct
{
    hi_mutex_t mutex;
#if _HI_PTHREAD
    pthread_cond_t *cond;
    int semaphore_count;
#endif
}hi_semaphore_t;

extern void hi_mutex_init(hi_mutex_t *mutex);
extern void hi_mutex_lock(hi_mutex_t *mutex);
extern void hi_mutex_unlock(hi_mutex_t *mutex);
extern void hi_mutex_deinit(hi_mutex_t *mutex);

extern void hi_recursive_mutex_init(hi_mutex_t *mutex);
extern void hi_recursive_mutex_lock(hi_mutex_t *mutex);
extern void hi_recursive_mutex_unlock(hi_mutex_t *mutex);
extern void hi_recursive_mutex_deinit(hi_mutex_t *mutex);

extern void hi_semaphore_init(hi_semaphore_t *semaphore);
extern void hi_semaphore_wait(hi_semaphore_t *semaphore);
extern void hi_semaphore_signal(hi_semaphore_t *semaphore);
extern void hi_semaphore_deinit(hi_semaphore_t *semaphore);

typedef uint8_t hi_priority_t;
typedef enum{
    HI_PRIORITY_BACKGROUND = 0,   //for data storage or something not important.
    HI_PRIORITY_NORMAL = 1,   //usually task.
    HI_PRIORITY_RESPONSE = 2, //for response user input.
    HI_PRIORITY_DISPLAY = 3,  //for display sync.
    HI_PRIORITY_CRITICAL = 4, //for critical safety function.
}hi_priority_enum;

#if _HI_FREERTOS
// typedef TaskFunction_t hi_thread_func_f;
typedef void* (*hi_thread_func_f)(void* args);
#else
typedef void* (*hi_thread_func_f)(void* args);
#endif

typedef struct
{
#if _HI_PTHREAD
    pthread_t handle;
#elif _HI_FREERTOS
    // TaskHandle_t handle;
    void* handle;
#else
    uint32_t handle;
#endif
    hi_str_t name;
    hi_thread_func_f func;
    uint32_t stack_depth;   //stack depth for thread.
    hi_priority_t priority;   //the priority for thread.
    hi_value_t args;
}hi_thread_t;

#ifndef HI_THREAD_DEFAULT
#define HI_THREAD_DEFAULT(__name__, __func__) { \
    .name = __name__,   \
    .func = (hi_thread_func_f)__func__,   \
    .stack_depth = 0,    \
    .priority = HI_PRIORITY_NORMAL, \
    .args = HI_VALUE_NULL,   \
}
#endif

extern int hi_thread_init(hi_thread_t *thread);
extern void hi_thread_deinit(void);
extern void hi_sleep(hi_time_t ms);

#ifdef __cplusplus

#endif

#endif /* HI_THREAD_H_ */
