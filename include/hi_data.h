/**
 * @file hi_data.h
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

#ifndef HI_DATA_H_
#define HI_DATA_H_

#include "hi_defines.h"
#include "hi_object.h"

#ifdef __cplusplus
extern "C" {
#endif

HI_CLASS_DEF(hi_data, hi_object)
    hi_size_t size;
    hi_ptr_t data;
HI_CLASS_END

HI_CLASS_COPYING_DEF(hi_data)

void hi_data_append(hi_data* object, hi_data* append);

// void hi_data_init(hi_data_t *data);
// void hi_data_copy(hi_data_t *source, hi_data_t *dist);
// void hi_data_deinit(hi_data_t *data);

#ifdef __cplusplus
}
#endif

#endif
