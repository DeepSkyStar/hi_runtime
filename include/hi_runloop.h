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

#include "hi_defines.h"
#include "hi_method_map.h"
#include "hi_error.h"
#include "hi_async_queue.h"
#include "hi_string.h"

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
    hi_str_t name;
    uint32_t stack_depth;   //stack depth for runloop task.
    hi_priority_t priority;   //the priority for runloop task.
    uint64_t periods;    //how many period of this runloop run.
    hi_ticks_t frequency; //0 means no delay.
    uint64_t ticks; //how many ticks when run.
    hi_async_queue_t events; //recv queue for input event.
    hi_runloop_func init_func;   //will run once when start a runloop.
    hi_runloop_func loop_func;    //will run many time in 1s, depends on frequency.
    hi_runloop_func end_func;  //will run once after end.
};

//Default setup for a runloop.
void hi_runloop_default(hi_runloop_t *runloop, hi_str_t name, hi_runloop_func func);

hi_runloop_t *hi_runloop_main(void);

void hi_runloop_start(hi_runloop_t *runloop);
void hi_runloop_run(hi_runloop_t *runloop);
void hi_runloop_stop(hi_runloop_t *runloop);

hi_err_t hi_runloop_send(hi_runloop_t *runloop, void *item, hi_ticks_t ticks_to_wait);
hi_err_t hi_runloop_send_fromISR(hi_runloop_t *runloop, void *item, hi_ticks_t ticks_to_wait);
hi_err_t hi_runloop_recv(hi_runloop_t *runloop, void *item, hi_ticks_t ticks_to_wait);

#ifdef __cplusplus
}
#endif

#endif
