/**
 * @file hi_osal.c
 * @author Cosmade (deepskystar@outlook.com)
 * @brief 
 * @version
 * @date 2024-08-01
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
#include "hi_osal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if _HI_ESP32
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#elif _HI_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#else
#endif

#if _HI_PTHREAD
#include <sys/time.h>
#endif

hi_time_t __default_hi_get_time(void)
{
#if _HI_PTHREAD
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (hi_time_t)tv.tv_sec * (hi_time_t)1000 + (hi_time_t)tv.tv_usec / (hi_time_t)1000;
#elif _HI_FREERTOS
    return (hi_time_t)xTaskGetTickCount() * (hi_time_t)portTICK_PERIOD_MS;
#else
    return 0;
#endif
}

void __default_hi_sleep(hi_time_t ms)
{
#if _HI_PTHREAD
    usleep((uint64_t)ms*(uint64_t)1000);
#elif _HI_FREERTOS
    vTaskDelay(ms / portTICK_PERIOD_MS);
#else
    return;
#endif
}

hi_osal_t* hi_osal(void)
{
    static hi_osal_t __shared_osal = {
        .mem = {
            .memset_imp = memset,
            .memcpy_imp = memcpy,
            .memmove_imp = memmove,
            .malloc_imp = malloc,
            .realloc_imp = realloc,
            .free_imp = free,
        },
        .time = {
            .get_time_imp = __default_hi_get_time,
        },
        .thread = {
            .sleep_imp = __default_hi_sleep,
        }
    };
    return &__shared_osal;
}