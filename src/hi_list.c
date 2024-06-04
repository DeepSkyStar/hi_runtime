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
#include "hi_log.h"

inline hi_list_t* hi_list_new(hi_size_t data_size, hi_size_t max_size)
{
    hi_list_t *list = hi_malloc(sizeof(hi_list_t));
    hi_mem_pool_t *pool = hi_mem_pool_new((hi_mem_pool_config_t){
        .use_check = 1,
        .is_dynamic = 1,
        .block_size = data_size,
        .block_count = max_size
    });
    hi_list_init(list, pool);
    return list;
}

inline void hi_list_init(hi_list_t *list, hi_mem_pool_t *pool)
{
    if (list->pool != NULL)
    {
        hi_list_deinit(list);
    }
    list->pool = pool;
}

inline void hi_list_deinit(hi_list_t *list)
{
    hi_list_del_all(list);
    hi_mem_pool_free(list->pool);
    list->pool = NULL;
}

inline void hi_list_free(hi_list_t *list)
{
    hi_list_deinit(list);
    hi_free(list);
}

inline hi_iter_t hi_list_add_value(hi_list_t *list, hi_value_t value)
{
    hi_iter_t iter = hi_mem_pool_take(list->pool);
    if (iter == HI_ITER_NULL) return iter;
    *(hi_value_t*)hi_mem_pool_get(list->pool, iter) = value;
    return iter;
}

inline hi_iter_t hi_list_add(hi_list_t *list, const void* data, hi_size_t size)
{
    if (list->pool == NULL || list->pool->config.block_size < size) {
        HI_LOGE("the size bigger than list pool block size!");
        return HI_ITER_NULL;
    }
    hi_iter_t iter = hi_mem_pool_take(list->pool);
    if (iter == HI_ITER_NULL) return iter;
    hi_memcpy(hi_list_get(list, iter), data, size);
    return iter;
}

inline void* hi_list_get(hi_list_t *list, hi_iter_t index)
{
    return hi_mem_pool_get(list->pool, index);
}

inline void hi_list_del(hi_list_t *list, hi_iter_t index)
{
    hi_mem_pool_bring(list->pool, index);
}

inline void hi_list_del_all(hi_list_t *list)
{
    hi_mem_pool_bring_all(list->pool);
}

inline hi_iter_t hi_list_begin(hi_list_t *list)
{
    for (hi_iter_t iter = 0; iter < list->pool->cur;iter += list->pool->config.block_size)
    {
        if (hi_mem_pool_get(list->pool, iter) != NULL)
        {   
            return iter;
        }
    }
    return HI_ITER_NULL;
}

inline hi_iter_t hi_list_next(hi_list_t *list, hi_iter_t index)
{
    index += list->pool->config.block_size;
    while (index < list->pool->cur)
    {
        if (hi_mem_pool_get(list->pool, index) != NULL)
        {
            return index;
        }
        index += list->pool->config.block_size;
    }
    return HI_ITER_NULL;
}
