/**
 * @file hi_list.c
 * @author Cosmade (deepskystar@outlook.com)
 * @brief 
 * @version
 * @date 2024-06-03
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

#include "hi_list.h"

inline void hi_list_init(hi_list_t *list, hi_mem_block_pool_t *pool)
{
    list->pool = pool;
}

inline hi_iter_t hi_list_add(hi_list_t *list, hi_value_t value)
{
    hi_iter_t iter = hi_mem_block_pool_take(list->pool);
    if (iter == HI_ITER_NULL) return iter;
    *(hi_value_t*)hi_mem_block_pool_get(list->pool, iter) = value;
    return iter;
}


inline hi_iter_t hi_list_add_data(hi_list_t *list, const void* data, hi_size_t size)
{
    hi_iter_t iter = hi_mem_block_pool_take(list->pool);
    if (iter == HI_ITER_NULL) return iter;
    hi_memcpy(hi_list_get(list, iter), data, size);
    return iter;
}

inline void* hi_list_get(hi_list_t *list, hi_iter_t index)
{
    return hi_mem_block_pool_get(list->pool, index);
}

inline void hi_list_del(hi_list_t *list, hi_iter_t index)
{
    hi_mem_block_pool_bring(list->pool, index);
}

inline hi_iter_t hi_list_begin(hi_list_t *list)
{
    for (hi_iter_t iter = 0; iter < list->pool->cur;iter += list->pool->config.block_size)
    {
        if (hi_mem_block_pool_get(list->pool, iter) != NULL)
        {
            return iter;
        }
    }
    return HI_ITER_NULL;
}

inline hi_iter_t hi_list_next(hi_list_t *list, hi_iter_t index)
{
    for (; index < list->pool->cur;index += list->pool->config.block_size)
    {
        if (hi_mem_block_pool_get(list->pool, index) != NULL)
        {
            return index;
        }
    }
    return HI_ITER_NULL;
}