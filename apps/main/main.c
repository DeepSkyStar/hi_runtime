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

HI_CLASS_DEF(sample_obj, hi_object)
    uint8_t start[0];
    uint8_t a;
    uint32_t b;
    uint8_t end[0];
HI_CLASS_END

HI_CLASS_IMP(sample_obj, hi_object)

HI_CLASS_INIT_IMP_EMPTY(sample_obj);

HI_CLASS_DEINIT_IMP_EMPTY(sample_obj);

void test_obj()
{
    sample_obj *a = HI_NEW(sample_obj);
    a->a = 10;

    sample_obj *b = HI_RETAIN(a);

    HI_RELEASE(a);
    b = NULL;

    HI_RELEASE(a);

    HI_RELEASE(a);
    HI_RELEASE(a);

    hi_pair_t pair = {
        .key = {.str_val = "123"},
        .value = {.str_val = "sdfdsf"}
    };

   printf("%s: %s\n", pair.key.str_val, (char*)pair.value.str_val);
   printf("%zu\n", (size_t)-1);
}

void test_mem()
{   
    int max_pool_capa = 32;
    hi_size_t test_index[32] = {0};
    hi_loop_pool_t* pool = hi_loop_pool_new(sizeof(sample_obj), max_pool_capa);
    
    for (int i = 0; i < max_pool_capa; i++)
    {
        test_index[i] = hi_loop_pool_take_unit(pool);
        sample_obj* ptr = (sample_obj*)(pool->vector + test_index[i]);
        ptr->a = i;
        ptr->b = i;
        printf("<%d> First Get Unit: %zu \n",i , test_index[i]);
    }
    
    for (int i = 0; i< max_pool_capa; i++)
    {
        hi_loop_pool_bring_unit(pool, test_index[i]);
    }
    
    for (int i = 0; i < max_pool_capa; i++)
    {
        test_index[i] = hi_loop_pool_take_unit(pool);
        sample_obj* ptr = (sample_obj*)(pool->vector + test_index[i]);
        ptr->a = i;
        ptr->b = i;
        printf("<%d> Second Get Unit: %zu \n",i , test_index[i]);
    }
}

int main(int argc, char *argv[])
{

    printf("This system is: %d\n", hi_get_os());

    test_obj();
    test_mem();

    return 0;
}
