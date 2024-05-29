/**
 * @file hi_log.h
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

#ifndef HI_LOG_H_
#define HI_LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hi_sys.h"
#include "hi_types.h"

/************************************** Halo Log ********************************************/

/**
 * @brief Log level
 *
 */
typedef enum hi_log_level_enum{
    HI_LOG_NONE,       /*!< No log output */
    HI_LOG_ERROR,      /*!< Critical errors, software module can not recover on its own */
    HI_LOG_WARN,       /*!< Error conditions from which recovery measures have been taken */
    HI_LOG_INFO,       /*!< Information messages which describe normal flow of events */
    HI_LOG_DEBUG,      /*!< Extra information which is not necessary for normal use (values, pointers, sizes, etc). */
    HI_LOG_VERBOSE     /*!< Bigger chunks of debugging information, or frequent messages which can potentially flood the output. */
} hi_log_level_t;

#ifndef BOOTLOADER_BUILD

#if _HI_ESP32

#if defined(__cplusplus) && (__cplusplus >  201703L)
#define HI_LOGE( format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR,   __FILE__, format __VA_OPT__(,) __VA_ARGS__)
#define HI_LOGW( format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_WARN,    __FILE__, format __VA_OPT__(,) __VA_ARGS__)
#define HI_LOGI( format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO,    __FILE__, format __VA_OPT__(,) __VA_ARGS__)
#define HI_LOGD( format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_DEBUG,   __FILE__, format __VA_OPT__(,) __VA_ARGS__)
#define HI_LOGV( format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_VERBOSE, __FILE__, format __VA_OPT__(,) __VA_ARGS__)
#else // !(defined(__cplusplus) && (__cplusplus >  201703L))
#define HI_LOGE( format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_ERROR,   __FILE__, format, ##__VA_ARGS__)
#define HI_LOGW( format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_WARN,    __FILE__, format, ##__VA_ARGS__)
#define HI_LOGI( format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_INFO,    __FILE__, format, ##__VA_ARGS__)
#define HI_LOGD( format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_DEBUG,   __FILE__, format, ##__VA_ARGS__)
#define HI_LOGV( format, ... ) ESP_LOG_LEVEL_LOCAL(ESP_LOG_VERBOSE, __FILE__, format, ##__VA_ARGS__)
#endif // !(defined(__cplusplus) && (__cplusplus >  201703L))

#else

#define HI_LOGE( format, ... ) printf( "<ERROR>%s:%d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define HI_LOGW( format, ... ) printf( "<WARNING>%s:%d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define HI_LOGI( format, ... ) printf( "<INFO>%s:%d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define HI_LOGD( format, ... ) printf( "<DEBUG>%s:%d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#define HI_LOGV( format, ... ) printf( "<VERBOSE>%s:%d: " format "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#endif  /* HI_IS_ESP32 */

#else
#define HI_LOGE( format, ... ) 
#define HI_LOGW( format, ... ) 
#define HI_LOGI( format, ... ) 
#define HI_LOGD( format, ... ) 
#define HI_LOGV( format, ... ) 
#endif

#ifndef HI_CHECK_ERR
#define HI_CHECK_ERR(__check__, __str__, ...)                  \
    do                                                                            \
    {                                                                             \
        if ((__check__).res != HI_RESULT_OK)                                      \
        {                                                                         \
            HI_LOGE("%s(%d) REASON:%x TIPS: " __str__, __FUNCTION__, __LINE__, (__check__).reason , ##__VA_ARGS__); \
            return __check__;                                                   \
        }                                                                         \
    } while (0)
#endif

#ifndef HI_ERR_FROM_ESP32
#define HI_ERR_FROM_ESP32(__var__, __esp32_err__) \
    do                                                                                              \
    {                                                                                               \
        (__var__).reason = __esp32_err__;                                                           \
        (__var__).res = ((__var__).reason==0)? HI_RESULT_OK: HI_RESULT_FAILED;                        \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif
