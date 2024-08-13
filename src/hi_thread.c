/**
 * @file hi_thread.c
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
#include "hi_thread.h"
#include "hi_osal.h"
#include "hi_memory.h"
#include "hi_log.h"

#if _HI_ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#elif _HI_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#else
#endif

#ifdef CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT
#define HI_THREAD_DEFAULT_STACK_SIZE CONFIG_PTHREAD_TASK_STACK_SIZE_DEFAULT
#else

#ifdef configMINIMAL_STACK_SIZE
#define HI_THREAD_DEFAULT_STACK_SIZE configMINIMAL_STACK_SIZE
#else
#define HI_THREAD_DEFAULT_STACK_SIZE (1024 * 2)
#endif

#endif

inline void hi_mutex_init(hi_mutex_t *mutex)
{
#if _HI_PTHREAD
    mutex->mutex = hi_malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(mutex->mutex, NULL);
#elif _HI_FREERTOS
    mutex->mutex = xSemaphoreCreateMutex();
#else
    return ;
#endif
}

inline void hi_mutex_lock(hi_mutex_t *mutex)
{
#if _HI_PTHREAD
    pthread_mutex_lock(mutex->mutex);
#elif _HI_FREERTOS
    xSemaphoreTake((SemaphoreHandle_t)mutex->mutex, portMAX_DELAY);
#else
    return ;
#endif
}

inline void hi_mutex_unlock(hi_mutex_t *mutex)
{
#if _HI_PTHREAD
    pthread_mutex_unlock(mutex->mutex);
#elif _HI_FREERTOS
    xSemaphoreGive((SemaphoreHandle_t)mutex->mutex);
#else
    return ;
#endif
}

inline void hi_mutex_deinit(hi_mutex_t *mutex)
{
#if _HI_PTHREAD
    pthread_mutex_destroy(mutex->mutex);
    hi_free(mutex->mutex);
    mutex->mutex = NULL;
#elif _HI_FREERTOS
    vSemaphoreDelete((SemaphoreHandle_t)mutex->mutex);
    mutex->mutex = NULL;
#else
    return ;
#endif
}

inline void hi_recursive_mutex_init(hi_mutex_t *mutex)
{
#if _HI_PTHREAD
    mutex->mutex = hi_malloc(sizeof(pthread_mutex_t));
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(mutex->mutex, &attr);
#elif _HI_FREERTOS
    mutex->mutex = xSemaphoreCreateRecursiveMutex();
#else
    return ;
#endif
}

inline void hi_recursive_mutex_lock(hi_mutex_t *mutex)
{
#if _HI_PTHREAD
    pthread_mutex_lock(mutex->mutex);
#elif _HI_FREERTOS
    // xSemaphoreTake(mutex->mutex, portMAX_DELAY);
    xSemaphoreTakeRecursive(mutex->mutex, portMAX_DELAY);
#else
    return ;
#endif
}

inline void hi_recursive_mutex_unlock(hi_mutex_t *mutex)
{
#if _HI_PTHREAD
    pthread_mutex_unlock(mutex->mutex);
#elif _HI_FREERTOS
    // xSemaphoreGive(mutex->mutex);
    xSemaphoreGiveRecursive(mutex->mutex);
#else
    return ;
#endif
}

inline void hi_recursive_mutex_deinit(hi_mutex_t *mutex)
{
#if _HI_PTHREAD
    pthread_mutex_destroy(mutex->mutex);
    hi_free(mutex->mutex);
    mutex->mutex = NULL;
#elif _HI_FREERTOS
    vSemaphoreDelete(mutex->mutex);
    mutex->mutex = NULL;
#else
    return ;
#endif
}

void hi_semaphore_init(hi_semaphore_t *semaphore)
{
#if _HI_PTHREAD
    hi_mutex_init(&(semaphore->mutex));
    semaphore->cond = hi_malloc(sizeof(pthread_cond_t));
    pthread_cond_init(semaphore->cond, NULL);
    semaphore->semaphore_count = 0;
#elif _HI_FREERTOS
    semaphore->mutex.mutex = xSemaphoreCreateBinary();
#else
    return ;
#endif
}

void hi_semaphore_wait(hi_semaphore_t *semaphore)
{
#if _HI_PTHREAD
    pthread_mutex_lock(semaphore->mutex.mutex);
    pthread_cond_wait(semaphore->cond, semaphore->mutex.mutex); // 等待条件变量
    pthread_mutex_unlock(semaphore->mutex.mutex);
#elif _HI_FREERTOS
    xSemaphoreTake(semaphore->mutex.mutex, portMAX_DELAY);
#else
    return ;
#endif
}

void hi_semaphore_signal(hi_semaphore_t *semaphore)
{
#if _HI_PTHREAD
    pthread_mutex_lock(semaphore->mutex.mutex);
    pthread_cond_signal(semaphore->cond);
    pthread_mutex_unlock(semaphore->mutex.mutex);
#elif _HI_FREERTOS
    xSemaphoreGive(semaphore->mutex.mutex);
#else
    return ;
#endif
}

void hi_semaphore_deinit(hi_semaphore_t *semaphore)
{
#if _HI_PTHREAD
    hi_mutex_deinit(&(semaphore->mutex));
    pthread_cond_destroy(semaphore->cond);
    semaphore->cond = NULL;
#elif _HI_FREERTOS
    vSemaphoreDelete(semaphore->mutex.mutex);
    semaphore->mutex.mutex = NULL;
#else
    return ;
#endif
}

inline int hi_thread_init(hi_thread_t *thread)
{
#if _HI_PTHREAD
    //TODO: fill out thread attr.
    return pthread_create(&(thread->handle), NULL, thread->func, thread->args.byte);
#elif _HI_FREERTOS
    //TODO: make translate from hi_priority to freertos.
    return xTaskCreate((TaskFunction_t)thread->func,
                thread->name,
                (thread->stack_depth == 0)? HI_THREAD_DEFAULT_STACK_SIZE: thread->stack_depth,
                thread->args.byte,
                thread->priority,
                thread->handle);
#else
    return -1;
#endif
}

inline int hi_thread_join(hi_thread_t *thread)
{
#if _HI_PTHREAD
    //TODO: fill out thread attr.
    return pthread_join(thread->handle, NULL);
#elif _HI_FREERTOS
    return ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
#else
    return -1;
#endif
}

inline void hi_thread_deinit(void)
{
#if _HI_PTHREAD
    //TODO: test it.
    pthread_exit(NULL);
#elif _HI_FREERTOS
    vTaskDelete(NULL);
#else
    return;
#endif
}

inline void hi_sleep(hi_time_t ms)
{
    hi_osal->thread.sleep_imp(ms);
}
