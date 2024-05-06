/**
 * @file hi_algorithm.h
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

#ifndef HI_ALGORITHM_H_
#define HI_ALGORITHM_H_

#include "hi_defines.h"
#include "hi_memory.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    hi_size_t size;
    hi_ptr_t data;
}hi_data_t;

typedef enum
{
    HI_VALUE_TYPE_PTR = 0,
    HI_VALUE_TYPE_OFFSET,
    HI_VALUE_TYPE_STR,
    HI_VALUE_TYPE_UINT,
    HI_VALUE_TYPE_INT,
    HI_VALUE_TYPE_FLOAT,
}hi_value_type;

typedef union
{
    void* ptr_val;
    hi_size_t ptr_offset;
    hi_str_t str_val;
    uint32_t uint_val;
    int32_t int_val;
    float float_val;
}hi_value_t;

#define HI_VALUE_NULL ((hi_value_t)NULL)
#define HI_IS_VALUE_NULL(__value__) (__value__.uint_val == 0)

typedef struct
{
    hi_value_t key;
    hi_value_t value;
}hi_pair_t;

typedef struct hi_link_node_s hi_link_node_t;
struct hi_link_node_s
{
    hi_link_node_t *next;
    hi_value_t value;
};  //Two ptr.

typedef struct hi_dual_node_s hi_dual_node_t;
struct hi_dual_node_s
{
    hi_dual_node_t *l;
    hi_dual_node_t *r;
    hi_value_t value;
};

typedef struct
{
    hi_link_node_t* head;   //top
    hi_link_node_t* tail;   //bottom
    hi_size_t size; //The size of usage.
    hi_loop_pool_t* pool;   //use memory pool for storage.
}hi_link_t;

typedef struct
{
    hi_link_node_t *head;
    hi_link_node_t *tail;
    hi_link_t meta;
}hi_queue_t;

typedef struct
{
    hi_size_t size;
    hi_value_t value[0];
}hi_vector_t;

typedef struct
{
    hi_pair_t node;
}hi_map_t;

typedef struct
{
    hi_pair_t node;
}hi_hashmap_t;

//This method assume this pointer is not NULL.
extern void hi_algo_vector_new(hi_vector_t* vector, hi_size_t size);
extern void hi_algo_vector_free(hi_vector_t* vector);
extern void hi_algo_vector_resize(hi_vector_t* vector, hi_size_t size);
extern void hi_algo_vector_set(hi_vector_t* vector, hi_size_t index, hi_value_t value);
extern hi_value_t hi_algo_vector_get(hi_vector_t* vector, hi_size_t index);

//If want to used fixed memory, set capacity to 0, and init the pool pointer before call this function.
//If capacity is 0, and pool pointer is NULL, it will alloc memory when memory full.
// extern void hi_algo_link_new(hi_link_t* link, hi_size_t capacity);
// extern void hi_algo_link_free(hi_link_t* link);
// //This function will realloc memory.
// extern void hi_algo_link_realloc(hi_link_t* link, hi_size_t capacity);
// extern uint8_t hi_algo_is_link_full(hi_link_t *link);
extern void hi_algo_link_add(hi_link_t* link, hi_size_t index, hi_value_t value);
extern void hi_algo_link_set(hi_link_t* link, hi_size_t index, hi_value_t value);
extern void hi_algo_link_del(hi_link_t* link, hi_size_t index);
extern hi_value_t hi_algo_link_get(hi_link_t* link, hi_size_t index);

//This method assume this pointer is not NULL.
extern void hi_algo_queue_new(hi_queue_t* queue);
extern void hi_algo_queue_free(hi_queue_t* queue);
extern void hi_algo_queue_add(hi_queue_t* queue, hi_size_t index, hi_value_t value);
extern void hi_algo_queue_add_head(hi_queue_t* queue, hi_value_t value);
extern void hi_algo_queue_add_tail(hi_queue_t* queue, hi_value_t value);
extern void hi_algo_queue_set(hi_queue_t* queue, hi_size_t index, hi_value_t value);
extern void hi_algo_queue_set_head(hi_queue_t* queue, hi_value_t value);
extern void hi_algo_queue_set_tail(hi_queue_t* queue, hi_value_t value);
extern void hi_algo_queue_del(hi_queue_t* queue, hi_size_t index);
extern void hi_algo_queue_del_head(hi_queue_t* queue);
extern void hi_algo_queue_del_tail(hi_queue_t* queue);
extern hi_value_t hi_algo_queue_get(hi_queue_t* queue, hi_size_t index);
extern hi_value_t hi_algo_queue_get_head(hi_queue_t* queue);
extern hi_value_t hi_algo_queue_get_tail(hi_queue_t* queue);

//This method assume this pointer is not NULL.
extern void hi_algo_hashmap_new(hi_map_t* map);
extern void hi_algo_hashmap_free(hi_map_t* map);
extern void hi_algo_hashmap_set(hi_map_t* map, hi_value_t key, hi_value_t value);
extern hi_value_t hi_algo_hashmap_get(hi_map_t* map, hi_value_t key);
extern void hi_algo_hashmap_del(hi_map_t* map, hi_value_t key);

//This method assume this pointer is not NULL.
extern void hi_algo_map_new(hi_map_t* map);
extern void hi_algo_map_free(hi_map_t* map);
extern void hi_algo_map_set(hi_map_t* map, hi_value_t key, hi_value_t value);
extern hi_value_t hi_algo_map_get(hi_map_t* map, hi_value_t key);
extern void hi_algo_map_del(hi_map_t* map, hi_value_t key);

#ifdef __cplusplus
}
#endif

#endif
