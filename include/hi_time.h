/**
 * @file hi_time.h
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

#ifndef HI_TIME_H_
#define HI_TIME_H_

#include "hi_sys.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief 32bit for millionseconds only 49 days.
 *      64bit for nanoseconds, near 210406 days.
 * 
 * @return hi_precision_time_t 
 */

#if _HI_SUPPORT_64BIT
typedef uint64_t hi_time_t;
#define HI_TIME_MAX (UINT64_MAX)
#else
typedef uint32_t hi_time_t;
#define HI_TIME_MAX (UINT32_MAX)
#endif

// typedef uint32_t hi_ticks_t;
/**
 * @brief some system not allow. will return 0.
 * 
 * @return hi_ticks_t 
 */
// extern hi_ticks_t hi_get_ticks(void);

/**
 * @brief unit is ms.
 * 
 * @return hi_time_t 
 */
extern hi_time_t hi_get_time(void);

#ifdef __cplusplus
}
#endif

#endif
