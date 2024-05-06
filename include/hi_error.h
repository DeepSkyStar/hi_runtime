/**
 * @file hi_error.h
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

#ifndef HI_ERROR_H_
#define HI_ERROR_H_

#include "hi_defines.h"
#include "hi_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

/************************************** Halo Error ********************************************/
typedef struct hi_err_s hi_err_t;

typedef enum{
    HI_ERR_OK = 0,
    HI_ERR_FAILED = 1,
    HI_ERR_TIMEOUT = 2,
    HI_ERR_CANCEL = 3,
}hi_err_code;

typedef enum{
    HI_ERR_REASON_NONE = 0,
    HI_ERR_REASON_NOT_SUPPORT = 1,
    HI_ERR_REASON_PARAM = 2,
    HI_ERR_REASON_MEMORY = 3,
    HI_ERR_REASON_EMPTY = 4,
    HI_ERR_REASON_FULL = 5,
    HI_ERR_REASON_SYSTEM = 6,
    HI_ERR_REASON_COUNT,
    HI_ERR_REASON_UNKOWN = 0xFFFF,
}hi_err_runtime_reason;

typedef enum{
    HI_ERR_DOMAIN_RUNTIME = 0,
    HI_ERR_DOMAIN_SYSTEM = 1,
}hi_err_domain;

struct hi_err_s{
    hi_err_code type:2;
    uint16_t domain:14; //used to identify the reason. the custom domain must be avoid hi_err_domain.
    uint16_t reason;
};

//halo err ok const
extern const hi_err_t HI_ERR_OK_CONST;
extern const hi_err_t HI_ERR_FAILED_CONST;
extern const hi_err_t HI_ERR_TIMEOUT_CONST;
extern const hi_err_t HI_ERR_CANCEL_CONST;

#pragma pack()

#ifndef HI_CHECK_ERR
#define HI_CHECK_ERR(__check__, __str__, ...)                  \
    do                                                                            \
    {                                                                             \
        if ((__check__).type != HI_ERR_OK)                                      \
        {                                                                         \
            HI_LOGE("%s(%d) DOMAIN:%x REASON:%x TIPS: " __str__, __FUNCTION__, __LINE__, (__check__).domain, (__check__).reason , ##__VA_ARGS__); \
            return __check__;                                                   \
        }                                                                         \
    } while (0)
#endif

#ifndef HI_ERR_FROM_ESP32
#define HI_ERR_FROM_ESP32(__var__, __esp32_err__) \
    do                                                                                              \
    {                                                                                               \
        (__var__).domain = HI_ERR_DOMAIN_SYSTEM;                                                  \
        (__var__).reason = __esp32_err__;                                                           \
        (__var__).type = ((__var__).reason==0)? HI_ERR_OK: HI_ERR_FAILED;                        \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif