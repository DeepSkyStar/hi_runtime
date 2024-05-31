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

#ifdef __cplusplus
extern "C" {
#endif

#define HI_RUNLOOP_DEFAULT_STACK_DEPTH (2048 * 2)
#define HI_RUNLOOP_DEFAULT_QUEUE_LEN (10)
#define HI_RUNLOOP_DEFAULT_QUEUE_ITEM_SIZE (sizeof(uint32_t))

typedef struct hi_runloop_state_s hi_runloop_state_t;
typedef struct hi_runloop_s hi_runloop_t;

typedef void (*hi_runloop_func)(hi_runloop_t* runloop);

struct hi_runloop_state_s{
    uint8_t is_running:1;
    uint8_t reserved:7;
};

struct hi_runloop_s{
    hi_runloop_state_t state;
    hi_ticks_t frequency; //0 means no delay.
    hi_time_t running_time; //the time for run.
    uint64_t periods;    //how many period of this runloop run.
    uint64_t ticks; //how many ticks when run.

    void *owner;    //for runloop owner.

    hi_thread_t thread;   //thread
    hi_buffer_t buffer;   //buffer for read and write.

    hi_runloop_func init_func;   //will run once when start a runloop.
    hi_runloop_func loop_func;    //will run many time in 1s, depends on frequency.
    hi_runloop_func end_func;  //will run once after end.
};

//Default setup for a runloop.
#define HI_RUNLOOP_DEFAULT(__owner__, __name__ , __func__) {    \
    .state = {0},   \
    .frequency = 0, \
    .periods = 0,   \
    .owner = __owner__, \
    .thread = HI_THREAD_DEFAULT(__name__, __func__),    \
    .buffer = HI_BUFFER_DEFAULT,    \
    .loop_func = __func__,  \
}

hi_runloop_t *hi_runloop_main(void);

void hi_runloop_start(hi_runloop_t *runloop);
void hi_runloop_run(hi_runloop_t *runloop);
void hi_runloop_stop(hi_runloop_t *runloop);

// hi_result_t hi_runloop_send(hi_runloop_t *runloop, void *item, hi_ticks_t ticks_to_wait);

// hi_result_t hi_runloop_send_fromISR(hi_runloop_t *runloop, void *item, hi_ticks_t ticks_to_wait);

// hi_size_t hi_runloop_send(hi_runloop_t *runloop, void *data, hi_size_t size);
// hi_result_t hi_runloop_recv(hi_runloop_t *runloop, void *data, hi_ticks_t ticks_to_wait);

hi_result_t hi_runloop_add_task(hi_runloop_t *runloop, hi_task_t *task);
void hi_runloop_cancel_task(hi_runloop_t *runloop, hi_task_t *task);

#ifdef __cplusplus
}
#endif

#endif
