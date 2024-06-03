/**
 * @file hi_runloop.h
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

#ifndef HI_RUNLOOP_H_
#define HI_RUNLOOP_H_

#include "hi_sys.h"
#include "hi_types.h"
#include "hi_isr_queue.h"
#include "hi_task.h"
#include "hi_thread.h"
#include "hi_queue.h"
#include "hi_map.h"
#include "hi_buffer.h"
#include "hi_time.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HI_RUNLOOP_DEFAULT_STACK_DEPTH (2048 * 2)
#define HI_RUNLOOP_DEFAULT_QUEUE_LEN (10)
#define HI_RUNLOOP_DEFAULT_QUEUE_ITEM_SIZE (sizeof(uint32_t))

typedef struct hi_runloop_s hi_runloop_t;

typedef void (*hi_runloop_func)(hi_runloop_t* runloop);

typedef struct
{
    uint8_t is_running:1;
    uint8_t reserved:7;

    hi_time_t start_time;   //the time for run. if become zero will change.
    hi_time_t running_time;   //the time for run. if become zero will change.
    hi_time_t exp_time;   //expected time for run. if become zero will change.
    hi_time_t period;    //how many period of this runloop run.

    hi_mutex_t thread_mutex;   //the mutex for runloop run.
}hi_runloop_state_t;

struct hi_runloop_s{
    hi_runloop_state_t _state;  //the runloop state, should not be modify or fetch it directly

    void *owner;    //the thread holder.
    hi_thread_t thread;   //thread, will alloc in every start.
    hi_time_t interval; //the interval for one loop. 0 means directly.

    hi_mutex_t queue_mutex; //mutex for task queue.
    hi_queue_t queue;   //task queue.

    hi_runloop_func init_func;   //will run once when start a runloop.
    hi_runloop_func loop_func;    //will run many time in 1s, depends on frequency.
    hi_runloop_func end_func;  //will run once after end.
};

//Default setup for a runloop.
#define HI_RUNLOOP_INIT(__name__ , __interval__, __func__) {    \
    ._state = (hi_runloop_state_t){0},   \
    .owner = NULL, \
    .interval = __interval__, \
    .thread = HI_THREAD_DEFAULT(__name__, hi_runloop_main),    \
    .loop_func = __func__,  \
}

//The first time used must init.
void hi_runloop_init(hi_runloop_t *runloop);
void hi_runloop_start(hi_runloop_t *runloop);
void hi_runloop_wait(hi_runloop_t *runloop);
void hi_runloop_stop(hi_runloop_t *runloop);

/**
 * @brief If you call this directly, it wiil run the runloop in current thread.
 * 
 * @param runloop 
 */
void* hi_runloop_main(hi_runloop_t *runloop);

uint8_t hi_runloop_is_running(hi_runloop_t *runloop);
hi_time_t hi_runloop_running_time(hi_runloop_t *runloop);

hi_result_t hi_runloop_add_task(hi_runloop_t *runloop, hi_task_t task);

#ifdef __cplusplus
}
#endif

#endif
