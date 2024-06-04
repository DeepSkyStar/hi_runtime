/**
 * @file hi_time.c
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

#include "hi_time.h"

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

// inline hi_ticks_t hi_get_ticks(void)
// {
// #if _HI_FREERTOS
//     return xTaskGetTickCount();
// #else
//     return 0;
// #endif
// }

inline hi_time_t hi_get_time(void)
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
