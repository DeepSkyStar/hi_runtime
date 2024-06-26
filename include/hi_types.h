/**
 * @file hi_types.h
 * @author Cosmade (deepskystar@outlook.com)
 * @brief 
 * @version
 * @date 2024-05-27
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

#ifndef HI_TYPES_H_
#define HI_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_sys.h"

#define HI_MAX_SIZE ((hi_size_t)-1)
#define HI_ITER_NULL HI_MAX_SIZE
#define HI_SAFE_ITER(__iter__) ((__iter__)&(~(hi_iter_t)1))
#define HI_ITER_INVALID(__iter__) (__iter__)^=1
#define HI_ITER_IS_INVALID(__iter__) ((__iter__)&1)

#define HI_OFFSET_OF(__type__, __member__) ((size_t) &((__type__ *)0)->__member__)
#define HI_CONTAINER_OF(ptr, type, member) ({           \
const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
(type *)( (char *)__mptr - HI_OFFSET_OF(type, member) );})


typedef size_t hi_size_t;
typedef hi_size_t hi_iter_t;

#pragma pack(1)

/************************* Error ****************************/

typedef enum{
    HI_RESULT_OK = 0,
    HI_RESULT_FAILED = 1,
    HI_RESULT_TIMEOUT = 2,
    HI_RESULT_CANCELLED = 3,
}hi_result_e;

typedef enum{
    HI_FAILED_REASON_NONE = 0, //means ok/unknown.
    HI_FAILED_REASON_DISCONNECTED = 1,  //sending data when disconnected.
    HI_FAILED_REASON_NOT_SUPPORT = 2,  //protocol not support this operation.
    HI_FAILED_REASON_WRONG_PARAM = 3,  //wrong param format.
    HI_FAILED_REASON_NOT_EXIST = 4,    //not exist api or obj. 
    HI_FAILED_REASON_NO_PERMISSION = 5,    //no permission. before authrozied.
    HI_FAILED_REASON_OUT_OF_MEMORY = 6,    //out of memory. when performan some api.
    HI_FAILED_REASON_OUT_OF_STORAGE = 7,   //out of storage. when saving data.
    HI_FAILED_REASON_OUT_OF_INDEX = 8,   //out of index. when listing data.
    HI_FAILED_REASON_OUT_OF_MTU = 9,       //out of mtu, should modify code.
    HI_FAILED_REASON_NULL_PTR = 10, //null pointer
    HI_FAILED_REASON_SYSTEM = 11, //system failure
    HI_FAILED_REASON_HARDWARE = 12, //hardware failure
}hi_failed_reason_e;

typedef struct{
    uint8_t res:2; //See: hi_result_e
    uint8_t reason:6;   //See: hi_failed_reason_e
}hi_result_t;

#define HI_RESULT_MAKE(__res__, __reason__) (hi_result_t){__res__, __reason__}
#define HI_RESULT_MAKE_OK HI_RESULT_MAKE(HI_RESULT_OK, 0)
#define HI_RESULT_MAKE_FAILED(__reason__) HI_RESULT_MAKE(HI_RESULT_FAILED, __reason__)
#define HI_RESULT_MAKE_TIMEOUT HI_RESULT_MAKE(HI_RESULT_TIMEOUT, 0)
#define HI_RESULT_MAKE_CANCEL HI_RESULT_MAKE(HI_RESULT_CANCELLED, 0)

#pragma pack()

/************************* Value ****************************/

typedef enum
{
    HI_VALUE_TYPE_NULL      = 0,
    HI_VALUE_TYPE_BYTE      = 1,
    HI_VALUE_TYPE_FILE      = 2,
    HI_VALUE_TYPE_DIR       = 3,
    HI_VALUE_TYPE_UINT8     = 4,
    HI_VALUE_TYPE_INT8      = 5,
    HI_VALUE_TYPE_UINT16    = 6,
    HI_VALUE_TYPE_INT16     = 7,
    HI_VALUE_TYPE_UINT32    = 8,
    HI_VALUE_TYPE_INT32     = 9,
    HI_VALUE_TYPE_FLOAT32   = 10,

#if _HI_SUPPORT_64BIT
    HI_VALUE_TYPE_UINT64    = 11,
    HI_VALUE_TYPE_INT64     = 12,
    HI_VALUE_TYPE_FLOAT64   = 13,
#endif
}hi_value_type_e;

typedef union
{
    hi_iter_t iter;
    char* str;
    void* byte;
    uint8_t uint8;
    uint8_t *uint8_list;
    int8_t int8;
    uint8_t *int8_list;
    uint16_t uint16;
    uint16_t *uint16_list;
    int16_t int16;
    int16_t *int16_list;
    uint32_t uint32;
    uint32_t *uint32_list;
    int32_t int32;
    int32_t *int32_list;
    float float32;
    float *float32_list;

#if _HI_SUPPORT_64BIT
    uint64_t uint64;
    uint64_t *uint64_list;
    int64_t int64;
    int64_t *int64_list;
    double float64;
    double *float64_list;

    uint8_t raw[8];
#else
    uint8_t raw[4];
#endif
}hi_value_t;

typedef struct{
    hi_result_t result;
    hi_value_t value;
}hi_value_result_t;

#define HI_VALUE(__value__) ((hi_value_t *)(__value__))
#define HI_VALUE_ITER(__value__) (hi_value_t){.iter = __value__}
#define HI_VALUE_STR(__value__) (hi_value_t){.str = __value__}
#define HI_VALUE_BYTE(__value__) (hi_value_t){.byte = __value__}
#define HI_VALUE_FLOAT(__value__) (hi_value_t){.float32 = __value__}

#if _HI_SUPPORT_64BIT

#define HI_VALUE_INT(__value__) (hi_value_t){.int64 = __value__}
#define HI_VALUE_UINT(__value__) (hi_value_t){.uint64 = __value__}
#define HI_VALUE_DOUBLE(__value__) (hi_value_t){.float64 = __value__}

#else

#define HI_VALUE_INT(__value__) (hi_value_t){.int32 = __value__}
#define HI_VALUE_UINT(__value__) (hi_value_t){.uint32 = __value__}

#endif

#define HI_VALUE_NULL ((hi_value_t){.byte = NULL})
#define HI_IS_VALUE_NULL(__value__) (__value__.byte == NULL)

typedef struct
{
    hi_value_t data;
    hi_size_t size;
}hi_data_t;

#define HI_DATA_NULL ((hi_data_t){.size = 0, .data = HI_VALUE_NULL})
#define HI_IS_DATA_NULL(__data__) (__data__.size == 0 || HI_IS_VALUE_NULL(__value__))

typedef char* hi_str_t;

#ifdef __cplusplus
}
#endif

#endif /* HI_TYPES_H_ */
