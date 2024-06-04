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

// #pragma pack(1)

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

// #pragma pack()

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

    // HI_RELEASE(a);

    // HI_RELEASE(a);
    // HI_RELEASE(a);

    // hi_pair_t pair = {
    //     .key = {.byte = {"123"}},
    //     .value = {.byte = {"sdfdsf"}}
    // };

//    printf("%s: %s\n", (char *)pair.key.byte.ptr, (char *)pair.value.byte.ptr);
//    printf("%zu\n", (size_t)-1);
}

#define TEST_CLASS test_mem_t
#define TEST_BLOCK_CNT (10)

HI_MEM_POOL_DEFINE(test_mem_pool, sizeof(test_mem_t), TEST_BLOCK_CNT);

void test_mem()
{   
    printf("actuall block size is: %d\n", test_mem_pool.config.block_size);
    printf("sample obj size: %lu\n", sizeof(TEST_CLASS));
    hi_iter_t test_index[TEST_BLOCK_CNT];
    for (int i = 0; i < TEST_BLOCK_CNT; i++)
    {
        test_index[i] = hi_mem_pool_take(&test_mem_pool);
        TEST_CLASS* ptr = (TEST_CLASS*)hi_mem_pool_get(&test_mem_pool, test_index[i]);
        // ptr->a = i;
        // ptr->b = i;
        printf("<%d> First Get Unit: %zu used: %d\n", i, test_index[i], HI_MEM_POOL_IS_IN_USE(&test_mem_pool, test_index[i]));
        
    }

    for (int i = 0; i < TEST_BLOCK_CNT / 2; i++)
    {
        printf("<%d> before bring block: %zu used: %d\n", i, test_index[i], HI_MEM_POOL_IS_IN_USE(&test_mem_pool, test_index[i]));
        hi_mem_pool_bring(&test_mem_pool, test_index[i]);
        printf("<%d> bring block: %zu used: %d\n", i, test_index[i], HI_MEM_POOL_IS_IN_USE(&test_mem_pool, test_index[i]));

        // for (int j = 0; j < TEST_BLOCK_CNT; j++)
        // {
        //     int result = (hi_mem_pool_get(&test_mem_pool, test_index[j]) != NULL);
        //     printf("<%d> in use:%d\n", j, result);
        // }
    }
    
    for (int j = 0; j < TEST_BLOCK_CNT; j++)
    {
        printf("<%d> in use:%d\n", j, hi_mem_pool_get(&test_mem_pool, test_index[j]) != NULL);
    }

    for (int i = 0; i < TEST_BLOCK_CNT / 2; i++)
    {
        hi_iter_t iter = hi_mem_pool_take(&test_mem_pool); 
        TEST_CLASS* ptr = (TEST_CLASS*)(hi_mem_pool_get(&test_mem_pool, iter));
        // ptr->a = i;
        // ptr->b = i;
        printf("<%d> Second Get Unit: %zu \n",i , iter);
        for (int j = 0; j < TEST_BLOCK_CNT; j++)
        {
            int result = (hi_mem_pool_get(&test_mem_pool, test_index[j]) != NULL);
            printf("<%d> Second in use:%d\n", j, result);
        }
    }

    // for (int j = 0; j < TEST_BLOCK_CNT; j++)
    // {
    //     int result = (hi_mem_pool_get(&test_mem_pool, test_index[j]) != NULL);
    //     printf("<%d> Second in use:%d\n", j, result);
    // }
}

//2 0000, 3ms, for 80mhz is 100ms
//200 for 80mhz is 1ms
#define TEST_MAP_COUNT (1000000)

//20 0000, 2ms, for 80mhz is 100ms
//2000 for 80mhz is 1ms
#define TEST_QUEUE_COUNT (1000000)

HI_MAP_POOL_DEFINE(shared_map_pool, sizeof(hi_value_t), TEST_MAP_COUNT);
HI_QUEUE_POOL_DEFINE(shared_queue_pool, sizeof(hi_value_t), TEST_QUEUE_COUNT);

void print_tab(int num)
{
    for (int i = 0; i < num; i++)
    {
        printf("\t");
    }
}

void print_map(hi_map_t *map)
{
    if (map->root == HI_ITER_NULL) return;
    hi_queue_t queue = HI_QUEUE_INIT(&shared_queue_pool);
    hi_queue_t next_queue = HI_QUEUE_INIT(&shared_queue_pool);

    hi_queue_add_last_value(&queue, HI_VALUE_ITER(HI_MAP_ITER(map->root)));

    hi_size_t depth = hi_map_depth(map);
    hi_size_t blank = 0;
    printf("\n");
    printf("print depth: %zu\n", depth);
    while (hi_queue_begin(&queue) != HI_ITER_NULL)
    {
        blank = 0;
        while (hi_queue_begin(&queue) != HI_ITER_NULL)
        {
            hi_iter_t head = *(hi_iter_t *)hi_queue_get_first(&queue);
            if (hi_map_get_node(map, head)->left != HI_ITER_NULL)
            {
                
                hi_queue_add_last_value(&next_queue, HI_VALUE_ITER(HI_MAP_ITER(hi_map_get_node(map, head)->left)));
            }
            if (hi_map_get_node(map, head)->right != HI_ITER_NULL)
            {
                hi_queue_add_last_value(&next_queue, HI_VALUE_ITER(HI_MAP_ITER(hi_map_get_node(map, head)->right)));
            }

            hi_iter_t parent = hi_map_get_node(map, head)->parent;
            hi_map_key_t parent_key = HI_VALUE_NULL;
            if (parent != HI_ITER_NULL) parent_key = hi_map_get_node(map, parent)->key;
            if (HI_MAP_NODE_COLOR(map, head)) 
            {
                printf("%llu:%llu(B)\t", parent_key.uint64, hi_map_get_node(map, head)->key.uint64);
            }
            else
            {
                printf("%llu:%llu(R)\t", parent_key.uint64, hi_map_get_node(map, head)->key.uint64);
            }
            hi_queue_del_first(&queue);
        }
        printf("\n");
        hi_queue_t tmp = queue;
        queue = next_queue;
        // printf("<check:%llu>", hi_queue_head(next_queue).uint64);
        next_queue = tmp;
    }
    printf("\n");

}

void test_sync_queue(void)
{
    printf("start test async queue\n");

    hi_sync_queue_t *sync_queue = hi_malloc(sizeof(hi_sync_queue_t));
    sync_queue->unsafe.pool = &shared_queue_pool;
    HI_LOGD("start init");
    hi_sync_queue_init(sync_queue);

    HI_LOGD("stop init");
    hi_time_t last_time = hi_get_time();

    for (int i = 0;i < 10000000; i++)
    {
        hi_sync_queue_add_last_value(sync_queue, HI_VALUE_INT(i));
    }

    printf("used time: %llu\n", hi_get_time() - last_time);

    last_time = hi_get_time();

    
    while (hi_sync_queue_end(sync_queue) != HI_ITER_NULL)
    {
        // printf("get first:%d\n", ((hi_value_t *)hi_sync_queue_get_last(sync_queue))->int32);
        hi_sync_queue_del_last(sync_queue);
    }

    printf("used time: %llu\n", hi_get_time() - last_time);

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
        hi_queue_add_last_value(queue, HI_VALUE_INT(i));
    }

    printf("used time: %llu\n", hi_get_time() - last_time);

    while (hi_queue_begin(queue) != HI_ITER_NULL)
    {
        // printf("%d\n", HI_VALUE(hi_queue_get_first(queue))->int32);
        hi_queue_del_first(queue);
    }

    printf("used time: %llu\n", hi_get_time() - last_time);

    printf("end test queue\n\n");
}

void test_async_map()
{
    printf("start test map\n");
    hi_sync_map_t *map = hi_malloc(sizeof(hi_sync_map_t));
    map->unsafe.pool = &shared_map_pool;
    hi_sync_map_init(map);

    hi_time_t last_time = hi_get_time();

    for (int i = 0; i < TEST_MAP_COUNT + 1; i++)
    {
        hi_iter_t result = hi_sync_map_set_value(map, HI_VALUE_INT(i), HI_VALUE_INT(i));
        if (result == HI_ITER_NULL)
        {
            printf("the %d failed\n", i);
        }
    }

    printf("used time: %llu\n", hi_get_time() - last_time);

    hi_size_t depth = hi_sync_map_depth(map);
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
        hi_value_t *value = hi_sync_map_get(map, HI_VALUE_INT(i));
    }
    printf("used time: %llu\n", hi_get_time() - last_time);
    printf("end test map\n\n");
}


void test_map()
{
    printf("start test map\n");
    hi_map_t *map = hi_malloc(sizeof(hi_map_t));
    map->pool = &shared_map_pool;
    hi_map_init(map);

    hi_time_t last_time = hi_get_time();

    for (int i = 0; i < TEST_MAP_COUNT + 2; i++)
    {
        hi_iter_t result = hi_map_set_value(map, HI_VALUE_INT(i), HI_VALUE_INT(i));
        if (result == HI_ITER_NULL)
        {
            printf("the %d failed\n", i);
        }
    }

    printf("used time: %llu\n", hi_get_time() - last_time);

    hi_size_t depth = hi_map_depth(map);
    printf("after set, depth:%zu\n", depth);
    // print_map(map);
    printf("\n\n");


    int count = 0;
    for (hi_iter_t iter = hi_map_begin(map);iter != HI_ITER_NULL; iter = hi_map_next(map, iter))
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
        hi_map_set_value(map, HI_VALUE_INT(i), HI_VALUE_INT(i));
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
        hi_value_t *value = hi_map_get(map, HI_VALUE_INT(i));
        if (value == NULL)
        {
            printf("%d not found or its 0.\n", i);
        }
    }
    printf("used time: %llu\n", hi_get_time() - last_time);
    printf("end test map\n\n");
}

hi_result_t test_result()
{
    return (hi_result_t){HI_RESULT_TIMEOUT, 0};
}

typedef struct
{
    int a;
    int b;
}test_list_node_t;

HI_LIST_POOL_DEFINE(test_list_pool, sizeof(test_list_node_t), 100);
hi_list_t test_list = HI_LIST_INIT(&test_list_pool);

void test_list_func()
{
    for (int i = 0; i < 102; i++)
    {
        test_list_node_t node = {
            .a = i,
            .b = i
        };
        hi_iter_t iter = hi_list_add(&test_list, &node, sizeof(node));
        // HI_LOGD("add: %zu: %zu", iter, hi_list_get(&test_list, iter));

        // test_list_node_t *test_node = hi_list_get(&test_list, iter);
        HI_LOGD("add %zu",iter);
    }
    hi_iter_t iter = hi_list_begin(&test_list);
    int i = 0;
    for (int i = 0;iter!=HI_ITER_NULL && i < 102;iter = hi_list_next(&test_list, iter))
    {   
        test_list_node_t *test_node = hi_list_get(&test_list, iter);
        if (test_node != NULL)
        {
            HI_LOGD("this %d: %llu: %d %d", i, (uint64_t)test_node, test_node->a, test_node->b);
        }
        else
        {
            HI_LOGD("this: NULL");
        }
        i++;
    }
}

hi_sync_map_t lock_map = {.unsafe.pool = &shared_map_pool};

void test_lock()
{
    // hi_sync_map_init(&lock_map);
    // hi_sync_map_set_value(&lock_map, HI_VALUE_INT(1), HI_VALUE_INT(1));

    hi_mutex_t mutex;
    hi_mutex_init(&mutex);
    // hi_semaphore_t sem;
    // hi_semaphore_init(&sem);

    // HI_LOGD("start signal");
    // hi_semaphore_signal(&sem);

    // HI_LOGD("start wait");
    // hi_semaphore_wait(&sem);

    HI_LOGD("start lock");
    hi_mutex_lock(&mutex);

    HI_LOGD("start lock");
    hi_mutex_lock(&mutex);

    HI_LOGD("start unlock");
    hi_mutex_unlock(&mutex);
}

void test_runloop_func(hi_runloop_t *runloop)
{
    // uint64_t time = hi_get_time();
    HI_LOGI("current time is:%llu ms\n", hi_runloop_running_time(runloop));
    // HI_LOGI("current time is:%llu ms\n", hi_get_time());
    if (hi_runloop_running_time(runloop) >= 1000)
    {
        hi_runloop_stop(runloop);
    }
}

void test_runloop_func2(hi_runloop_t *runloop)
{
    // uint64_t time = hi_get_time();
    // HI_LOGI("current time is:%llu ms\n", hi_runloop_running_time(runloop));
    HI_LOGI("current time is:%llu ms\n", hi_get_time());
    if (hi_runloop_running_time(runloop) >= 2000)
    {
        hi_runloop_stop(runloop);
    }
}

hi_runloop_t shared_runloop = HI_RUNLOOP_INIT("test", 100, test_runloop_func);

hi_runloop_t shared_runloop2 = HI_RUNLOOP_INIT("test2", 100, test_runloop_func2);

void test_runloop()
{
    HI_LOGI("Init");
    hi_runloop_init(&shared_runloop);
    hi_runloop_init(&shared_runloop2);
    HI_LOGI("Start");
    hi_runloop_start(&shared_runloop);
    hi_runloop_start(&shared_runloop2);
    HI_LOGI("Wait");
    hi_runloop_wait(&shared_runloop);
    HI_LOGI("End1");
    hi_runloop_wait(&shared_runloop2);
    HI_LOGI("End2");
}

int main(int argc, char *argv[])
{
    printf("This system is: %d\n\n", hi_get_os());

    // test_obj();
    // test_mem();
    // test_sync_queue();
    // test_queue();
    test_map();
    // test_list_func();
    // test_async_map();
    // test_lock();
    // test_runloop();

    return 0;
}
