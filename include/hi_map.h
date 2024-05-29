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
    hi_iter_t root;
    hi_mem_pool_t *pool;    //the block size must bigger than hi_map_node_t, and can not be odd.
};

extern void hi_map_init(hi_map_t *map);

extern hi_result_t hi_map_set(hi_map_t *map, hi_map_key_t key, hi_value_t value);

extern hi_value_t hi_map_get(hi_map_t *map, hi_map_key_t key);

extern hi_iter_t hi_map_get_iter(hi_map_t *map, hi_map_key_t key);

extern hi_map_node_t hi_map_get_node(hi_map_t *map, hi_iter_t iter);

extern void hi_map_del(hi_map_t *map, hi_map_key_t key);

extern void hi_map_foreach(hi_map_t *map, hi_map_foreach_f func);

extern hi_size_t hi_map_depth(hi_map_t *map);

extern void hi_map_deinit(hi_map_t *map);

extern hi_iter_t hi_map_root(hi_map_t *map);
extern hi_iter_t hi_map_left(hi_map_t *map, hi_iter_t node);
extern hi_iter_t hi_map_right(hi_map_t *map, hi_iter_t node);
extern hi_iter_t hi_map_parent(hi_map_t *map, hi_iter_t node);


typedef struct
{
    hi_mutex_t mutex;
    hi_map_t unsafe;          //the block size must bigger than hi_map_node_t, and can not be odd.
}hi_aysnc_map_t;

extern void hi_aysnc_map_init(hi_aysnc_map_t *map);

extern hi_result_t hi_aysnc_map_set(hi_aysnc_map_t *map, hi_map_key_t key, hi_value_t value);

extern hi_value_t hi_aysnc_map_get(hi_aysnc_map_t *map, hi_map_key_t key);

extern void hi_aysnc_map_del(hi_aysnc_map_t *map, hi_map_key_t key);

extern hi_size_t hi_aysnc_map_depth(hi_aysnc_map_t *map);

extern void hi_aysnc_map_deinit(hi_aysnc_map_t *map);

#ifdef __cplusplus
}
#endif

#endif /* HI_MAP_H_ */
