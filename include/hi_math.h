/**
 * @file hi_math.h
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

#ifndef HI_MATH_H_
#define HI_MATH_H_

#include "hi_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MIN
#define MIN(__a__, __b__) ((__a__)<(__b__)?(__a__):(__b__))
#endif

#ifndef MAX
#define MAX(__a__, __b__) ((__a__)>(__b__)?(__a__):(__b__))
#endif

#ifndef HI_IS_ZERO
#define HI_IS_ZERO(__a__) ((__a__) < 1e-10 && -(__a__) > -1e-10)
#endif

#ifndef HI_IS_EQUAL
#define HI_IS_EQUAL(__a__, __b__) (HI_IS_ZERO(__a__ - __b__))
#endif

/******************************** Range for Calc ****************************************/

typedef struct hi_range16_s{
    uint16_t index;
    uint16_t length;
} hi_range16_t;

#define HALO_RANGE_END(range) ((range).index + (range).length)
#define IN_HALO_RANGE(range, i) ((i) >= (range).index && (i) < HALO_RANGE_END(range))

#ifdef __cplusplus
}
#endif

#endif