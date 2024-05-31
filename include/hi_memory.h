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

#include "hi_sys.h"
#include "hi_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HI_SETZERO
#define HI_SETZERO(__value__) hi_memset(&(__value__), 0, sizeof(__value__))
#endif

extern void* hi_memset(void* src, int value, hi_size_t size);
extern void* hi_memcpy(void* dst, const void* src, hi_size_t size);
extern void* hi_memmove(void* dst, const void* src, hi_size_t size);
extern void* hi_malloc(hi_size_t size);
extern void* hi_calloc(hi_size_t num, hi_size_t size);
extern void* hi_realloc(void* ptr, hi_size_t size);
extern void hi_free(void* ptr);

typedef struct
{
    //TODO: add variable and align support.
    hi_size_t variable:1;    //support variable length. default is 0.
    hi_size_t align:1;    //support memory alignment. default is 0.
    hi_size_t block_size:30;     //the single unit size of pool. only work when variable is 0.
    hi_size_t block_count; //the number of whole units.
}hi_mem_pool_config_t;

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
// HI_MEM_POOL_DEFINE(sample_pool, int, 100)
// 
// After this define, no need to call the new function, and sure cannot call realloc.
typedef struct
{
    hi_mem_pool_config_t config;
    hi_iter_t head;
    hi_iter_t tail;   //after memory use, memory will put here.
    hi_iter_t cur;    //for record where pool is using.
    uint8_t* container;    //the pool for data link.
}hi_mem_pool_t;

typedef struct
{
    uint32_t max_size;
    uint32_t usage;
}hi_var_mem_pool_t;

#define HI_MEM_POOL_INIT(__container__, __block_size__, __block_count__) { \
     .config = {        \
        .variable = 0,  \
        .align = _HI_ALIGN_MEM_POOL, \
        .block_size = __block_size__, \
        .block_count = __block_count__  \
    },  \
    .head = HI_ITER_NULL, \
    .tail = HI_ITER_NULL, \
    .cur = 0,   \
    .container = (uint8_t *)__container__,    \
}

#define HI_MEM_POOL_DEFINE(__name__, __type__, __count__) __type__ __name__##_container[__count__] = {0}; \
hi_mem_pool_t __name__ = HI_MEM_POOL_INIT(__name__##_container, sizeof(__type__), __count__)

/**
 * @brief alloc new mem pool. but in mcu, recommand to new a mem pool by hand.
 * 
 * @param config mem pool config.
 * @return hi_mem_pool_t* mem pool.
 */
extern hi_mem_pool_t* hi_mem_pool_new(hi_mem_pool_config_t config);

/**
 * @brief realloc mem pool. the block count must be bigger than before.
 * 
 * @param pool mem pool.
 * @param block_count block count.
 */
extern void hi_mem_pool_realloc(hi_mem_pool_t* pool, hi_size_t block_count);

/**
 * @brief Free mem pool, after that, all ref will invalid.
 * 
 * @param pool the mem pool.
 */
extern void hi_mem_pool_free(hi_mem_pool_t* pool);

/**
 * @brief if return HI_ITER_NULL, means full or not support unit.
 * 
 * @param pool mem pool.
 * @return hi_iter_t offset from the container.
 */
extern hi_iter_t hi_mem_pool_take(hi_mem_pool_t* pool);

/**
 * @brief MUST ensure each unit bring only once!!!
 * 
 * @param pool mem pool.
 * @param block offset from the container.
 */
extern void hi_mem_pool_bring(hi_mem_pool_t* pool, hi_iter_t block);

extern void* hi_var_mem_pool_take(hi_var_mem_pool_t* pool, hi_size_t size);
extern void hi_var_mem_pool_bring(hi_var_mem_pool_t* pool, void* data, hi_size_t size);

#ifdef __cplusplus
}
#endif

#endif