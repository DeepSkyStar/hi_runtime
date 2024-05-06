/**
 * @file hi_method_map.h
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

#ifndef HI_METHOD_MAP_H_
#define HI_METHOD_MAP_H_

#include "hi_defines.h"
#include "hi_error.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hi_method_map_s hi_method_map_t;
typedef struct hi_method_pair_s hi_method_pair_t;

typedef uint16_t hi_method_num_t;
typedef hi_method_num_t hi_method_key_t;

typedef void* (*hi_method_t)(void* args);

#define HI_METHOD_NULL (NULL)

#ifndef HI_METHOD_CALL
#define HI_METHOD_CALL(__map__, __key__, __type__, ...)               \
do                                                                      \
    {                                                                   \
        hi_method_t method = hi_method_map(__map__, __key__);       \
        if (method == HI_METHOD_NULL)                                 \
        {                                                               \
            HI_LOGE("%s(%d): method %d not exist" ,__FUNCTION__, __LINE__, __key__);\
                                                                        \
        }                                                               \
        else {                                                          \
            ((__type__)method)(##__VA_ARGS__);                          \
        }                                                               \
    } while (0)
#endif

#ifndef HI_METHOD_CALL_RET
#define HI_METHOD_CALL_RET(__map__, __key__, __type__, __ret__, ...)  \
do                                                                      \
    {                                                                   \
        hi_method_t method = hi_method_map(__map__, __key__);       \
        if (method == HI_METHOD_NULL)                                 \
        {                                                               \
            HI_LOGE("%s(%d): method %d not exist" ,__FUNCTION__, __LINE__, __key__);\
            __ret__ = HI_METHOD_NULL;                                 \
        }                                                               \
        else {                                                          \
            __ret__ = ((__type__)method)(##__VA_ARGS__);                \
        }                                                               \
    } while (0)
#endif

struct hi_method_pair_s
{
    hi_method_key_t key;
    hi_method_t method;
};

struct hi_method_map_s
{
    hi_method_num_t max_num;
    hi_method_pair_t map[0];
};

//Recommand to init in startup.
void hi_method_map_init(hi_method_map_t *map);
hi_err_t hi_method_register(hi_method_map_t *map, hi_method_key_t key, hi_method_t method);
hi_method_t hi_method_map(hi_method_map_t *map, hi_method_key_t key);

#ifdef __cplusplus
}
#endif

#endif