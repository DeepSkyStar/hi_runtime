/**
 * @file hi_buffer.h
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

#ifndef HI_BUFFER_H_
#define HI_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_sys.h"
#include "hi_types.h"
#include "hi_thread.h"

typedef struct
{
    hi_mutex_t mutex;
    hi_size_t max_size; //must bigger than mtu for process.
    hi_size_t cur_size;
    hi_iter_t cur;
    uint8_t* data;
}hi_buffer_t;


#define HI_BUFFER_DEFAULT_SIZE (2048)

#define HI_BUFFER_INIT(__container__, __size__) {  \
    .cur = 0,   \
    .cur_size = 0,  \
    .max_size = __size__,  \
    .data = __container__, \
}

#define HI_BUFFER_DEFINE(__name__, __size__) uint8_t __name__##_container[__size__] = {0}; \
hi_buffer_t __name__ = HI_BUFFER_INIT(__name__##_container, __size__)

void hi_buffer_init(hi_buffer_t *buffer);

/**
 * @brief read data to buffer.
 * 
 * @param buffer the buffer to add data.
 * @param data reading data.
 * @param size reading size data.
 * @return hi_size_t the size of data readed.
 */
hi_size_t hi_buffer_add(hi_buffer_t *buffer, const uint8_t *data, hi_size_t size);

void hi_buffer_lock(hi_buffer_t *buffer);
void hi_buffer_unlock(hi_buffer_t *buffer);

void hi_buffer_set_data(hi_buffer_t *buffer, const uint8_t *data, hi_size_t size);
void hi_buffer_clear(hi_buffer_t *buffer);

#ifdef __cplusplus
}
#endif

#endif /* HI_BUFFER_H_ */
