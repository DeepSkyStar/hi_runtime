/**
 * @file hi_memory.h
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

#ifndef HI_MEMORY_H_
#define HI_MEMORY_H_

#include "hi_defines.h"
#include "hi_error.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HI_SETZERO
#define HI_SETZERO(__value__) hi_memset(&(__value__), 0, sizeof(__value__))
#endif

#define HI_OFFSET_NULL (HI_MAX_SIZE)

extern void* hi_memset(void* src, int value, hi_size_t size);
extern void* hi_memcpy(void* src, void* dst, hi_size_t size);

extern void* hi_malloc(hi_size_t size);
extern void* hi_calloc(hi_size_t num, hi_size_t size);
extern void* hi_realloc(void* ptr, hi_size_t size);
extern void hi_free(void* ptr);

// There are three main memory allocation strategies:
//  1. Instant allocate Instant recovery. This strategy does NOT REQUIRE a memory pool.
//  2. Allocate a large chunk of memory at a time, and then allocate it when it is used up.  
//     This strategy REQUIRE a memory pool.
//  3. Allocate fixed size memory, and no longer allocate. This strategy REQUIRE a memory pool.
//
// For this purpose, build this struct for memory manager.
//
// If you want to define a static pool, just like that:
//
// #define UNIT_COUNT (100)
// int sample_pool_values[UNIT_COUNT] = {0};
// hi_loop_pool_t sample_pool = {
//     .head = NULL,
//     .tail = NULL,
//     .cur = 0,
//     .unit_size = sizeof(int),
//     .capacity = sizeof(int) * UNIT_COUNT,
//     .pool = sample_pool_values,
// };
//
//  or like that:
// 
//  hi_loop_pool_t sample_pool = {
//     .unit_size = sizeof(int),
//     .capacity = sizeof(int) * UNIT_COUNT,
//     .pool = sample_pool_values,
// };
// After this define, no need to call the new function, and sure cannot call realloc.
typedef struct
{
    hi_size_t head;
    hi_size_t tail;   //after memory use, memory will put here.
    hi_size_t cur;    //for record where pool is using.
    hi_size_t unit_size;     //the single unit size of pool.
    hi_size_t capacity; //the capacity of whole units.
    uint8_t vector[0];    //the pool for data link.
}hi_loop_pool_t;

extern hi_loop_pool_t* hi_loop_pool_new(hi_size_t unit_size, hi_size_t unit_count);
extern hi_loop_pool_t* hi_loop_pool_realloc(hi_loop_pool_t* pool, hi_size_t unit_count);
extern void hi_loop_pool_free(hi_loop_pool_t* pool);

//if return HI_OFFSET_NULL, means full.
extern hi_size_t hi_loop_pool_take_unit(hi_loop_pool_t* pool);

//MUST ensure each unit bring only once!!!
extern void hi_loop_pool_bring_unit(hi_loop_pool_t* pool, hi_size_t unit);

#ifdef __cplusplus
}
#endif

#endif