/**
 * @file hi_map.h
 * @author Cosmade (deepskystar@outlook.com)
 * @brief 
 * @version
 * @date 2024-05-29
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

#ifndef HI_MAP_H_
#define HI_MAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_sys.h"
#include "hi_types.h"
#include "hi_memory.h"
#include "hi_thread.h"

#define HI_MAP_BLACK (1)
#define HI_MAP_RED (0)
#define HI_MAP_ITER_COLOR(__iter__) ((__iter__)&((hi_iter_t)1))
#define HI_MAP_ITER(__iter__) (__iter__ == HI_ITER_NULL? __iter__:((__iter__)&(~(hi_iter_t)1)))
#define HI_MAP_NODE(__map__, __iter__) ((hi_map_node_t *)(hi_mem_block_pool_get((__map__)->pool, HI_MAP_ITER(__iter__))))
#define HI_MAP_NODE_COLOR(__map__, __iter__) ((__iter__) == HI_ITER_NULL? HI_MAP_BLACK: HI_MAP_ITER_COLOR(HI_MAP_NODE(__map__, __iter__)->parent))

typedef hi_value_t hi_map_key_t;
typedef struct hi_map_s hi_map_t;

typedef hi_result_t (*hi_map_foreach_f)(hi_map_t *map, hi_map_key_t key, hi_value_t value);

typedef struct
{
    hi_iter_t parent;
    hi_iter_t left;
    hi_iter_t right;
    hi_map_key_t key;
    hi_value_t value;
}hi_map_node_t;

struct hi_map_s
{
    hi_mem_block_pool_t *pool;    //the block size must bigger than hi_map_node_t, and can not be odd.
    hi_iter_t root;
};

#define HI_MAP_POOL_DEFINE(__name__, __count__) HI_MEM_POOL_DEFINE(__name__, sizeof(hi_map_node_t), __count__)

#define HI_MAP_INIT(__pool__) { \
    .pool = __pool__,   \
    .root = HI_ITER_NULL    \
}

extern void hi_map_init(hi_map_t *map, hi_mem_block_pool_t *pool);

extern hi_result_t hi_map_set(hi_map_t *map, hi_map_key_t key, hi_value_t value);

extern hi_value_t hi_map_get(hi_map_t *map, hi_map_key_t key);

extern void hi_map_del(hi_map_t *map, hi_map_key_t key);

extern hi_iter_t hi_map_first(hi_map_t *map);

extern hi_iter_t hi_map_next(hi_map_t *map, hi_iter_t node);

/**
 * @brief get iter from key. SHOULD notice that, the iter last bit will change sometime.
 * 
 * @param map map
 * @param key key the key for get map node.
 * @return hi_iter_t iter for raw used.
 */
extern hi_iter_t hi_map_get_iter(hi_map_t *map, hi_map_key_t key);

extern hi_map_node_t hi_map_node(hi_map_t *map, hi_iter_t iter);

extern void hi_map_foreach(hi_map_t *map, hi_map_foreach_f func);

extern hi_size_t hi_map_depth(hi_map_t *map);

extern void hi_map_deinit(hi_map_t *map);

/***************************************** Async Map ***************************************************/


typedef struct
{
    hi_mutex_t mutex;
    hi_map_t unsafe;          //the block size must bigger than hi_map_node_t, and can not be odd.
}hi_async_map_t;

/**
 * @brief async map must be init for use. before init, should setup the unsafe.pool for memory recovery.
 *  If the memory pool is share for some maps, should be careful.
 * 
 *  Recommand to setup one pool for one map.
 * 
 * @param map 
 */
extern void hi_async_map_init(hi_async_map_t *map, hi_mem_block_pool_t *pool);

extern hi_result_t hi_async_map_set(hi_async_map_t *map, hi_map_key_t key, hi_value_t value);

extern hi_value_result_t hi_async_map_get(hi_async_map_t *map, hi_map_key_t key);

extern void hi_async_map_del(hi_async_map_t *map, hi_map_key_t key);

extern hi_size_t hi_async_map_depth(hi_async_map_t *map);

extern void hi_async_map_deinit(hi_async_map_t *map);

#ifdef __cplusplus
}
#endif

#endif /* HI_MAP_H_ */
