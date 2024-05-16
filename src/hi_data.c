/**
 * @file hi_data.c
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
#include "hi_data.h"
#include "hi_log.h"
#include "hi_math.h"
#include "hi_memory.h"

HI_CLASS_IMP(hi_data, hi_object)

HI_CLASS_INIT_IMP(hi_data)
{
    object->size = 0;
    object->data = NULL;
    return object;
}

HI_CLASS_COPYING_IMP(hi_data)
{
    assert(src != NULL && dst != NULL);
    if (src->data == NULL || dst->data == NULL || src->size == 0 || dst->size == 0)
    {
        return dst;
    }
    hi_memcpy(dst->data, src->data, MIN(dst->size, src->size));
    return dst;
}

HI_CLASS_DEINIT_IMP(hi_data)
{
    if (object->data != NULL) 
    {
       hi_free(object->data);
    }
}

void hi_data_append(hi_data* object, hi_data* append)
{
    if (object->data == NULL) {

    }
}

// void hi_data_init(hi_data_t *data)
// {
//     assert(data != NULL);
//     if (data->data == NULL && data->len > 0) {
//         data->data = malloc(data->len);
//         memset(data, 0, sizeof(data->len));
//     }
// }

// void hi_data_copy(hi_data_t *source, hi_data_t *dist)
// {
//     assert(source != NULL && dist != NULL);
//     if (source->data == NULL || dist->data == NULL || source->len <= 0 || dist->len <= 0)
//     {
//         return;
//     }
//     memcpy(dist->data, source->data, MIN(dist->len, source->len));
// }

// void hi_data_deinit(hi_data_t *data)
// {
//     if (data != NULL) {
//         free(data->data);
//         data->len = 0;
//     }
// }
