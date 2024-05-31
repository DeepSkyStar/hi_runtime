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
#include <time.h>
#include "hi_runtime.h"

#pragma pack(1)

typedef struct
{
    uint8_t start[0];
    uint8_t a;
    uint32_t b;
    uint8_t end[0];
}test_mem_t;

HI_CLASS_DEF(sample_obj, hi_object)
    uint8_t start[0];
    uint8_t a;
    uint32_t b;
    uint8_t end[0];
HI_CLASS_END

#pragma pack()

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

    // hi_pair_t pair = {
    //     .key = {.byte = {"123"}},
    //     .value = {.byte = {"sdfdsf"}}
    // };

//    printf("%s: %s\n", (char *)pair.key.byte.ptr, (char *)pair.value.byte.ptr);
   printf("%zu\n", (size_t)-1);
}

#define TEST_CLASS test_mem_t
#define TEST_BLOCK_CNT (32)

void test_mem()
{   
    hi_size_t test_index[TEST_BLOCK_CNT] = {0};
    hi_mem_pool_config_t config = {
        .block_size = sizeof(TEST_CLASS),
        .block_count = TEST_BLOCK_CNT
    };
    hi_mem_pool_t* pool = hi_mem_pool_new(config);
    printf("actuall block size is: %d\n", pool->config.block_size);
    printf("sample obj size: %lu\n", sizeof(TEST_CLASS));
    for (int i = 0; i < TEST_BLOCK_CNT; i++)
    {
        test_index[i] = hi_mem_pool_take(pool);
        TEST_CLASS* ptr = (TEST_CLASS*)(pool->container + test_index[i]);
        ptr->a = i;
        ptr->b = i;
        printf("<%d> First Get Unit: %zu \n", i, test_index[i]);
    }

    for (int i = 0; i < TEST_BLOCK_CNT / 2; i++)
    {
        hi_mem_pool_bring(pool, test_index[i]);
        printf("<%d> bring block: %zu \n", i, test_index[i]);
    }
    
    for (int i = 0; i < TEST_BLOCK_CNT / 2; i++)
    {
        test_index[i] = hi_mem_pool_take(pool);
        TEST_CLASS* ptr = (TEST_CLASS*)(pool->container + test_index[i]);
        ptr->a = i;
        ptr->b = i;
        printf("<%d> Second Get Unit: %zu \n",i , test_index[i]);
    }
}

//2 0000, 3ms, for 80mhz is 100ms
//200 for 80mhz is 1ms
#define TEST_MAP_COUNT (1000000)

//20 0000, 2ms, for 80mhz is 100ms
//2000 for 80mhz is 1ms
#define TEST_QUEUE_COUNT (1000000)

HI_MEM_POOL_DEFINE(shared_map_pool, hi_map_node_t, TEST_MAP_COUNT);
HI_MEM_POOL_DEFINE(shared_queue_pool, hi_queue_node_t, TEST_QUEUE_COUNT);

void print_tab(int num)
{
    for (int i = 0; i < num; i++)
    {
        printf("\t");
    }
}

void print_map(hi_map_t *map)
{
    hi_queue_t *queue = hi_malloc(sizeof(hi_queue_t));
    queue->pool = &shared_queue_pool;
    hi_queue_t *next_queue = hi_malloc(sizeof(hi_queue_t));
    next_queue->pool = &shared_queue_pool;
    hi_queue_init(queue);
    hi_queue_init(next_queue);
    if (map->root == HI_ITER_NULL) return;

    hi_queue_in(queue, HI_VALUE_ITER(HI_MAP_ITER(map->root)));

    hi_size_t depth = hi_map_depth(map);
    hi_size_t blank = 0;
    printf("\n");
    printf("print depth: %zu\n", depth);
    while (queue->head != HI_ITER_NULL)
    {
        blank = 0;
        while (queue->head != HI_ITER_NULL)
        {
            hi_iter_t head = hi_queue_begin(queue);
            if (hi_map_node(map, head).left != HI_ITER_NULL)
            {
                hi_queue_in(next_queue, HI_VALUE_ITER(HI_MAP_ITER(hi_map_node(map, head).left)));
            }
            if (hi_map_node(map, head).right != HI_ITER_NULL)
            {
                hi_queue_in(next_queue, HI_VALUE_ITER(HI_MAP_ITER(hi_map_node(map, head).right)));
            }

            hi_iter_t parent = hi_map_node(map, head).parent;
            hi_map_key_t parent_key = HI_VALUE_NULL;
            if (parent != HI_ITER_NULL) parent_key = hi_map_node(map, parent).key;
            if (HI_MAP_NODE_COLOR(map, head)) 
            {
                printf("%llu:%llu(B)\t", parent_key.uint64, hi_map_node(map, head).key.uint64);
            }
            else
            {
                printf("%llu:%llu(R)\t", parent_key.uint64, hi_map_node(map, head).key.uint64);
            }
            hi_queue_out(queue);
        }
        printf("\n");
        hi_queue_t *tmp = queue;
        queue = next_queue;
        // printf("<check:%llu>", hi_queue_head(next_queue).uint64);
        next_queue = tmp;
    }
    printf("\n");
}

void test_async_queue(void)
{
    printf("start test async queue\n");

    hi_async_queue_t *async_queue = hi_malloc(sizeof(hi_async_queue_t));

    async_queue->unsafe.pool = &shared_queue_pool;
    hi_async_queue_init(async_queue);
    
    hi_time_t last_time = hi_get_time();

    for (int i = 0;i < TEST_QUEUE_COUNT; i++)
    {
        hi_async_queue_in(async_queue, HI_VALUE_INT(i));
    }

    printf("used time: %ld\n", hi_get_time() - last_time);

    last_time = hi_get_time();

    while (!hi_async_queue_empty(async_queue))
    {
        // printf("get last:%d\n", hi_async_queue_last(async_queue).int32);
        hi_async_queue_out(async_queue);
    }

    printf("used time: %ld\n", hi_get_time() - last_time);

    printf("end test async queue\n\n");
}

void test_queue()
{
    printf("start test queue\n");
    hi_queue_t *queue = hi_malloc(sizeof(hi_queue_t));
    queue->pool = &shared_queue_pool;
    hi_queue_init(queue);
    
    hi_time_t last_time = hi_get_time();

    for (int i = 0;i < TEST_QUEUE_COUNT; i++)
    {
        hi_queue_in(queue, HI_VALUE_INT(i));
    }

    printf("used time: %ld\n", hi_get_time() - last_time);

    while (queue->head != HI_ITER_NULL)
    {
        // printf("%d\n", hi_queue_head(queue).int32);
        hi_queue_out(queue);
    }

    printf("used time: %ld\n", hi_get_time() - last_time);

    printf("end test queue\n\n");
}

void test_async_map()
{
    printf("start test map\n");
    hi_async_map_t *map = hi_malloc(sizeof(hi_async_map_t));
    map->unsafe.pool = &shared_map_pool;
    hi_async_map_init(map);

    hi_time_t last_time = hi_get_time();

    for (int i = 0; i < TEST_MAP_COUNT + 1; i++)
    {
        hi_result_t result = hi_async_map_set(map, HI_VALUE_INT(i), HI_VALUE_INT(i));
        if (result.res != HI_RESULT_OK)
        {
            printf("the %d failed: %d\n", i, result.reason);
        }
    }

    printf("used time: %ld\n", hi_get_time() - last_time);

    hi_size_t depth = hi_async_map_depth(map);
    printf("print depth: %zu\n", depth);
    // print_map(map);
    printf("\n");

    // for (int i = 0; i < 4; i++)
    // {
    //     hi_map_del(map, HI_VALUE_INT(i));
    // }

    last_time = hi_get_time();
    for (int i = 0; i < TEST_MAP_COUNT; i++)
    {
        hi_value_result_t value = hi_async_map_get(map, HI_VALUE_INT(i));
    }
    printf("used time: %ld\n", hi_get_time() - last_time);
    printf("end test map\n\n");
}


void test_map()
{
    printf("start test map\n");
    hi_map_t *map = hi_malloc(sizeof(hi_map_t));
    map->pool = &shared_map_pool;
    hi_map_init(map);

    hi_time_t last_time = hi_get_time();

    for (int i = 0; i < TEST_MAP_COUNT + 1; i++)
    {
        hi_result_t result = hi_map_set(map, HI_VALUE_INT(i), HI_VALUE_INT(i));
        if (result.res != HI_RESULT_OK)
        {
            printf("the %d failed: %d\n", i, result.reason);
        }
    }

    printf("used time: %ld\n", hi_get_time() - last_time);

    hi_size_t depth = hi_map_depth(map);
    printf("after set\n");
    // print_map(map);
    printf("\n\n");


    int count = 0;
    for (hi_iter_t iter = hi_map_first(map);iter != HI_ITER_NULL; iter = hi_map_next(map, iter))
    {
        count++;
    }
    printf("Check count: %d\n", count);

    printf("start del\n");

    for (int i = 0; i < TEST_MAP_COUNT / 2; i++)
    {
        // printf("cur del %d depth: %zu\n", i, depth);
        hi_map_del(map, HI_VALUE_INT(i));
        // print_map(map);
        // printf("\n");
    }

    for (int i = 0; i < TEST_MAP_COUNT / 2; i++)
    {
        // printf("cur del %d depth: %zu\n", i, depth);
        // hi_map_del(map, HI_VALUE_INT(i));
        hi_map_set(map, HI_VALUE_INT(i), HI_VALUE_INT(i));
        // print_map(map);
        // printf("\n");
    }

    printf("after del\n");
    // print_map(map);
    printf("\n");
    printf("stop del\n\n");

    last_time = hi_get_time();
    for (int i = 0; i < TEST_MAP_COUNT; i++)
    {
        hi_value_t value = hi_map_get(map, HI_VALUE_INT(i));
        if (HI_IS_VALUE_NULL(value))
        {
            printf("%d not found or its 0.\n", i);
        }
    }
    printf("used time: %ld\n", hi_get_time() - last_time);
    printf("end test map\n\n");
}

hi_result_t test_result()
{
    return (hi_result_t){HI_RESULT_TIMEOUT, 0};
}

hi_async_map_t lock_map = {.unsafe.pool = &shared_map_pool};

void test_lock()
{
    // hi_async_map_init(&lock_map);
    // hi_async_map_set(&lock_map, HI_VALUE_INT(1), HI_VALUE_INT(1));

    hi_mutex_t mutex;
    hi_mutex_init(&mutex);

    HI_LOGD("start lock");
    hi_mutex_lock(&mutex);

    HI_LOGD("start lock");
    hi_mutex_lock(&mutex);

    HI_LOGD("start unlock");
    hi_mutex_unlock(&mutex);
}

int main(int argc, char *argv[])
{
    printf("This system is: %d\n\n", hi_get_os());

    // test_obj();
    // test_mem();
    // test_async_queue();
    // test_queue();
    // test_map();
    // test_async_map();
    test_lock();

    return 0;
}
