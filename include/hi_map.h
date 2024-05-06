/**
 * @file hi_map.h
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

#ifndef HI_MAP_H_
#define HI_MAP_H_

#include "hi_defines.h"
#include "hi_object.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _hi_map_node_s _hi_map_node_t;
struct _hi_map_node_s{
    void* key;
};

HI_CLASS_DEF(hi_map, hi_object)
HI_CLASS_END

// void hi_map_set(hi_str_t* key, hi_object* object);
// hi_object* hi_map_get(hi_str_t* key);

#ifdef __cplusplus
}
#endif

#endif /* HI_MAP_H_ */
