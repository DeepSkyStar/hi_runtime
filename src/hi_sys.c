/**
 * @file hi_sys.c
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

#include "hi_sys.h"
#if _HI_PTHREAD
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#endif

inline hi_sys_t hi_get_os(void)
{
#if _HI_NON_SYS
    return HI_OS_NON_SYS;
#elif _HI_FREERTOS
    return HI_OS_FREERTOR;
#elif _HI_WIN
    return HI_OS_WIN;
#elif _HI_LINUX
    return HI_OS_LINUX;
#elif _HI_ANDROID
    return HI_OS_ANDROID;
#elif _HI_MACOS
    return HI_OS_MACOS;
#elif _HI_IOS
    return HI_OS_IOS;
#elif _HI_TVOS
    return HI_OS_TVOS;
#elif _HI_WATCHOS
    return HI_OS_WATCHOS;
#else
    return HI_OS_NON_SYS;
#endif
}

void hi_print_stack(void)
{
#if _HI_PTHREAD
    void *array[10];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);

    printf("Stack trace:\n");
    for (i = 0; i < size; i++) {
        printf("%s\n", strings[i]);
    }

    free(strings);
#endif
}