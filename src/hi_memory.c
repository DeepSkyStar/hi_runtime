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

#include "hi_defines.h"
#include "hi_memory.h"

inline void* hi_memset(void* src, int value, hi_size_t size)
{
    return memset(src, value, size);
}

inline void* hi_memcpy(void* src, void* dst, hi_size_t size)
{
    return memcpy(src, dst, size);
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

inline hi_loop_pool_t* hi_loop_pool_new(hi_size_t unit_size, hi_size_t unit_count)
{
    if (unit_size < sizeof(hi_size_t)) unit_size = sizeof(hi_size_t);    //The minimun size for loop pool work.
    hi_loop_pool_t* pool = (hi_loop_pool_t*)hi_malloc(sizeof(hi_loop_pool_t) + unit_count * unit_size);
    pool->head = HI_MAX_SIZE;
    pool->tail = HI_MAX_SIZE;
    pool->cur = 0;
    pool->unit_size = unit_size;
    pool->capacity = unit_size * unit_count;
    return pool;
}

inline hi_loop_pool_t* hi_loop_pool_realloc(hi_loop_pool_t* pool, hi_size_t unit_count)
{
    pool = hi_realloc(pool, sizeof(hi_loop_pool_t) + pool->unit_size * unit_count);
    pool->capacity = pool->unit_size * unit_count;
    return pool;
}

inline void hi_loop_pool_free(hi_loop_pool_t* pool)
{
    hi_free(pool);
}

inline hi_size_t hi_loop_pool_take_unit(hi_loop_pool_t* pool)
{
    hi_size_t unit = HI_MAX_SIZE;
    if (pool->head != HI_MAX_SIZE) {
        unit = pool->head;
        pool->head = *(hi_size_t*)(pool->vector + pool->head);  //Get next pointer.
        return unit;
    }
    if (pool->cur < pool->capacity) {
        unit = pool->cur;
        pool->cur += pool->unit_size;
    }

    return unit;
}

inline void hi_loop_pool_bring_unit(hi_loop_pool_t* pool, hi_size_t unit)
{
    if (pool->head == HI_MAX_SIZE) {
        pool->head = unit;
        *((hi_size_t*)(pool->vector + unit)) = HI_MAX_SIZE;
        pool->tail = pool->head;
    }
    else {
        *(hi_size_t*)(pool->vector + pool->tail) = unit;
        pool->tail = unit;
        *((hi_size_t*)(pool->vector + unit)) = HI_MAX_SIZE;
    }
}