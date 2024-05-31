/**
 * @file hi_memory.c
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

#include "hi_memory.h"

inline void* hi_memset(void* src, int value, hi_size_t size)
{
    return memset(src, value, size);
}

inline void* hi_memcpy(void* dst, const void* src, hi_size_t size)
{
    return memcpy(dst, src, size);
}

inline void* hi_memmove(void* dst, const void* src, hi_size_t size)
{
    return memmove(dst, src, size);
}

inline void* hi_malloc(hi_size_t size)
{
    return malloc(size);
}

inline void* hi_calloc(hi_size_t num, hi_size_t size)
{
    return calloc(num, size);
}

inline void* hi_realloc(void* ptr, hi_size_t size)
{
    return realloc(ptr, size);
}

inline void hi_free(void* ptr)
{
    free(ptr);
}

inline hi_mem_pool_t* hi_mem_pool_new(hi_mem_pool_config_t config)
{
    //TODO: support align and variable.
    if (config.align || config.variable) return NULL;
    if (config.block_size < sizeof(hi_size_t)) {
        config.block_size = sizeof(hi_size_t);
    }
    //Make sure the block size is even.
    if ((config.block_size & 1) == 1) {
        config.block_size += 1;
    }
    //The minimun size for loop pool work.
    hi_mem_pool_t* pool = (hi_mem_pool_t*)hi_malloc(sizeof(hi_mem_pool_t) + config.block_size * config.block_count);
    pool->head = HI_ITER_NULL;
    pool->tail = HI_ITER_NULL;
    pool->cur = 0;
    pool->config = config;
    pool->container = (uint8_t *)pool + sizeof(hi_mem_pool_t);
    return pool;
}

inline void hi_mem_pool_realloc(hi_mem_pool_t* pool, hi_size_t block_count)
{
    pool = hi_realloc(pool, sizeof(hi_mem_pool_t) + pool->config.block_size * block_count);
    pool->config.block_count = block_count;
    pool->container = (uint8_t *)pool + sizeof(hi_mem_pool_t);
}

inline void hi_mem_pool_free(hi_mem_pool_t* pool)
{
    hi_free(pool);
}

inline hi_iter_t hi_mem_pool_take(hi_mem_pool_t* pool)
{
    hi_iter_t block = HI_ITER_NULL;
    if (pool->head != HI_ITER_NULL) {
        block = pool->head;
        pool->head = *(hi_iter_t*)(pool->container + HI_SAFE_ITER(pool->head));  //Get next pointer.
        return HI_SAFE_ITER(block);
    }
    if (pool->cur < pool->config.block_size * pool->config.block_count) {
        block = pool->cur;
        pool->cur += pool->config.block_size;
    }
    return block;
}

inline hi_iter_t hi_mem_pool_take_some(hi_mem_pool_t* pool, hi_size_t size)
{
    //TODO: not support variable mem pool yet.
    return HI_ITER_NULL;
}

inline void hi_mem_pool_bring(hi_mem_pool_t* pool, hi_iter_t block)
{
    //When block bring, it will reuse to storage the last block.
    if (pool->head == HI_ITER_NULL) {
        pool->head = HI_ITER_INVALID(block);
        *((hi_iter_t*)(pool->container + HI_SAFE_ITER(block))) = HI_ITER_NULL;
        pool->tail = pool->head;
    }
    else {
        *(hi_iter_t*)(pool->container + HI_SAFE_ITER(pool->tail)) = HI_ITER_INVALID(block);
        pool->tail = HI_ITER_INVALID(block);
        *((hi_iter_t*)(pool->container + HI_SAFE_ITER(block))) = HI_ITER_NULL;
    }
}