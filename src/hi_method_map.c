/**
 * @file hi_method_map.c
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
#include "hi_method_map.h"

#define HI_METHOD_MAX_INDEX (0xffff)

void hi_method_map_init(hi_method_map_t *map)
{
    int i = 0;
    for (;i < map->max_num; i++)
    {
        map->map[i].key = HI_METHOD_MAX_INDEX;
        map->map[i].method = HI_METHOD_NULL;
    }
}

hi_method_num_t hi_method_index(hi_method_map_t *map, hi_method_key_t key)
{
    hi_method_num_t index = key % map->max_num;
    hi_method_num_t last_index = index;
    while (map->map[index].key != HI_METHOD_MAX_INDEX && map->map[index].key != key)
    {
        index++;
        if (index >= map->max_num) {
            index = index % map->max_num;
        }
        if (last_index == index) 
        {
            return HI_METHOD_MAX_INDEX;
        }
    }
    return index;
}

hi_err_t hi_method_register(hi_method_map_t *map, hi_method_key_t key, hi_method_t method)
{
    hi_method_num_t index = hi_method_index(map, key);
    if (index >= map->max_num) return HI_ERR_FAILED_CONST;
    map->map[index].key = key;
    map->map[index].method = method;
    return HI_ERR_OK_CONST;
}

hi_method_t hi_method_map(hi_method_map_t *map, hi_method_key_t key)
{
    hi_method_num_t index = hi_method_index(map, key);
    if (index < map->max_num) {
        return map->map[index].method;
    }
    return HI_METHOD_NULL;
}