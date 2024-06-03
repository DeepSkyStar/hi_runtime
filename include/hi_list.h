/**
 * @file hi_list.h
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

#ifndef HI_LIST_H_
#define HI_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_sys.h"
#include "hi_types.h"
#include "hi_memory.h"

typedef struct
{
    hi_mem_block_pool_t *pool;
}hi_list_t;

#define HI_LIST_POOL_DEFINE(__pool__, __data_size__ ,__count__ ) HI_MEM_POOL_DEFINE(__pool__, __data_size__, __count__)
#define HI_LIST_INIT(__list__, __pool__) {\
    .pool = __pool__,   \
}

extern void hi_list_init(hi_list_t *list, hi_mem_block_pool_t *pool);

extern hi_iter_t hi_list_add(hi_list_t *list, hi_value_t value);
//must less then the list node size.
extern hi_iter_t hi_list_add_data(hi_list_t *list, const void* data, hi_size_t size);

extern void* hi_list_get(hi_list_t *list, hi_iter_t index);

extern void hi_list_del(hi_list_t *list, hi_iter_t index);

extern hi_iter_t hi_list_begin(hi_list_t *list);

extern hi_iter_t hi_list_next(hi_list_t *list, hi_iter_t index);

#ifdef __cplusplus
}
#endif

#endif /* HI_LIST_H_ */
