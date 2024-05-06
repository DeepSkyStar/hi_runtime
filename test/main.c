/**
 * @file main.c
 * @author Cosmade (deepskystar@outlook.com)
 * @brief A sample for test hi objc.
 * @version
 * @date 2024-05-04
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

#include <stdio.h>
#include "hi_runtime.h"

#define TEST_CNT (32)
hi_size_t test_index[TEST_CNT] = {0};

typedef struct
{
    uint8_t start[0];
    uint8_t a;
    uint32_t b;
    uint8_t end[0];
}test_t;

int main(int argc, char *argv[])
{
    // printf("Program name: %s\n", argv[0]);
    // if (argc > 1)
    // {
    //     for (int i = 1; i < argc; i++)
    //     {
    //         printf("Argument %d: %s\n", i, argv[i]);
    //     }
    // }
    // else
    // {
    //     printf("No arguments were provided.\n");
    // }
//    test_a *a = HI_NEW(test_a);
//
//    a->a = 10;
//
//    test_a *b = HI_RETAIN(a);
//
//    HI_RELEASE(a);
//    b = NULL;
//
//    HI_RELEASE(a);
//
//    HI_RELEASE(a);
//    HI_RELEASE(a);
//
//    hi_pair_t pair = {
//        .str = {s
//            .key = "123",
//            .value = "sdfdsf",
//        },
//    };

//    printf("%s: %s\n", pair.str.key, (char*)pair.str.value);
    
//    printf("%u\n", (size_t)-1);

    hi_loop_pool_t* pool = hi_loop_pool_new(sizeof(test_t), TEST_CNT);
    
    for (int i = 0; i < TEST_CNT; i++)
    {
        test_index[i] = hi_loop_pool_take_unit(pool);
        test_t* ptr = (test_t*)(pool->vector + test_index[i]);
        ptr->a = i;
        ptr->b = i;
        printf("<%d> First Get Unit: %d \n",i , test_index[i]);
    }
    
    for (int i = 0; i< TEST_CNT; i++)
    {
        hi_loop_pool_bring_unit(pool, test_index[i]);
    }
    
    for (int i = 0; i < TEST_CNT; i++)
    {
        test_index[i] = hi_loop_pool_take_unit(pool);
        test_t* ptr = (test_t*)(pool->vector + test_index[i]);
        ptr->a = i;
        ptr->b = i;
        printf("<%d> Second Get Unit: %d \n",i , test_index[i]);
    }
    return 0;
}
