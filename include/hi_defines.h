/**
 * @file hi_defines.h
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

#ifndef HI_DEFINES_H_
#define HI_DEFINES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#define HI_MAX_SIZE ((hi_size_t)-1)
typedef size_t hi_size_t;
typedef char* hi_str_t;
typedef void* hi_ptr_t;

//Chips & Systems Define Here.
//System and chip's define provide by hi_system.h.in, it will automatic generate the hi_system.h
// #define _HI_FOR_NON_SYS (0)
// #define _HI_FOR_FREERTOS (0)
// #define _HI_FOR_WIN64 (0)
// #define _HI_FOR_WIN32 (0)
// #define _HI_FOR_MACOS (0)
// #define _HI_FOR_IOS (0)
// #define _HI_FOR_ANDROID (0)
// #define _HI_FOR_LINUX (0)

#if HI_IS_ESP32
#include "esp_log.h"
#endif

#ifdef __cplusplus
}
#endif

#endif
