/**
 * @file hi_runloop.c
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

#include "hi_runloop.h"
#include "hi_time.h"

hi_runloop_t __main_runloop = {
    .name = "main",
    .frequency = 0,
    .priority = HI_PRIORITY_NORMAL,
    .stack_depth = HI_RUNLOOP_DEFAULT_STACK_DEPTH,
    .events.len = HI_RUNLOOP_DEFAULT_QUEUE_LEN,
    .events.item_size = HI_RUNLOOP_DEFAULT_QUEUE_ITEM_SIZE,
};

inline void hi_runloop_default(hi_runloop_t *runloop, hi_str_t name, hi_runloop_func func)
{
    runloop->name = name;
    runloop->loop_func = func;
    runloop->priority = HI_PRIORITY_NORMAL;
    runloop->stack_depth = HI_RUNLOOP_DEFAULT_STACK_DEPTH;
    runloop->state.is_running = 0;
    runloop->events.len = HI_RUNLOOP_DEFAULT_QUEUE_LEN;
    runloop->events.item_size = HI_RUNLOOP_DEFAULT_QUEUE_ITEM_SIZE;
    runloop->frequency = 0;
    runloop->ticks = 0;
    runloop->periods = 0;
}

inline hi_runloop_t *hi_runloop_main()
{
    return &__main_runloop;
}

void hi_runloop_run(hi_runloop_t *runloop)
{
    hi_isr_queue_init(&runloop->events);
    runloop->state.is_running = 1;
    runloop->ticks = 0;
    runloop->periods = 0;

    if (runloop->init_func) {
        runloop->init_func(runloop);
    }

    if (runloop->loop_func == NULL)
    {
        HI_LOGE("runloop has no loop func");
        goto END;
    }

    uint64_t cur_ticks = hi_get_ticks();
    uint64_t last_ticks = cur_ticks;

    while (runloop->state.is_running)
    {
        last_ticks = cur_ticks;
        cur_ticks = hi_get_ticks();

        //record ticks.
        if (cur_ticks >= last_ticks) {
            runloop->ticks = runloop->ticks + (cur_ticks - last_ticks);
        }
        else 
        {
            //if over 32bits time.
            runloop->ticks = runloop->ticks + (UINT32_MAX - last_ticks + cur_ticks);
        }

        runloop->loop_func(runloop);

        runloop->periods++;
//TODO: add more system support.
#if _HI_FREERTOS
        if (runloop->frequency == 0 || runloop->frequency >= configTICK_RATE_HZ) {
            continue;
        }
#endif

        last_ticks = cur_ticks;
        cur_ticks = hi_get_ticks();

        //record ticks.
        if (cur_ticks >= last_ticks) {
            runloop->ticks = runloop->ticks + (cur_ticks - last_ticks);
        }
        else 
        {
            //if over 32bits time.
            runloop->ticks = runloop->ticks + (UINT32_MAX - last_ticks + cur_ticks);
        }

        //if cur_time > expect time, pass
//TODO: add more system support.
#if _HI_FREERTOS
        if (runloop->periods * configTICK_RATE_HZ <= runloop->ticks * runloop->frequency)
        {
            continue;
        }
        vTaskDelay( (runloop->periods * configTICK_RATE_HZ - runloop->ticks * runloop->frequency) / runloop->frequency);
#endif
    }

END:
    if (runloop->end_func) {
        runloop->end_func(runloop);
    }
    hi_isr_queue_deinit(&runloop->events);
//TODO: add more system support.
#if _HI_FREERTOS
    vTaskDelete(NULL);
#endif
}

inline void hi_runloop_start(hi_runloop_t *runloop)
{
    if (runloop->state.is_running) return;
    runloop->state.is_running = 1;
//TODO: add more system support.
#if _HI_FREERTOS
    xTaskCreate((TaskFunction_t)hi_runloop_run, runloop->name, runloop->stack_depth, runloop, runloop->priority, NULL);
#endif
}

inline void hi_runloop_stop(hi_runloop_t *runloop)
{
    runloop->state.is_running = 0;
}

inline hi_result_t hi_runloop_send(hi_runloop_t *runloop, void *item, hi_ticks_t ticks_to_wait)
{
    return hi_isr_queue_send(&runloop->events, item, ticks_to_wait);
}

inline hi_result_t hi_runloop_send_fromISR(hi_runloop_t *runloop, void *item, hi_ticks_t ticks_to_wait)
{
    return hi_isr_queue_send_fromISR(&runloop->events, item, ticks_to_wait);
}

inline hi_result_t hi_runloop_recv(hi_runloop_t *runloop, void *item, hi_ticks_t ticks_to_wait)
{
    return hi_isr_queue_recv(&runloop->events, item, ticks_to_wait);
}
