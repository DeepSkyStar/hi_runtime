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

void* hi_runloop_main(hi_runloop_t *runloop)
{
    // hi_isr_queue_init(&runloop->events);
    // hi_mutex_lock(&(runloop->_state.thread_mutex));
    runloop->_state.is_running = 1;
    runloop->_state.start_time = hi_get_time();
    runloop->_state.running_time = 0;
    runloop->_state.exp_time = 0;
    runloop->_state.period = 0;

    if (runloop->init_func) {
        runloop->init_func(runloop);
    }

    if (runloop->loop_func == NULL)
    {
        HI_LOGE("runloop has no loop func");
        if (runloop->end_func) {
            runloop->end_func(runloop);
        }
        // hi_isr_queue_deinit(&runloop->events);
        hi_thread_deinit();
    }

    runloop->_state.running_time = hi_get_time() - runloop->_state.start_time;
    while (runloop->_state.is_running)
    {
        //the loop func.
        runloop->loop_func(runloop);
        runloop->_state.period++;
        
        runloop->_state.running_time = hi_get_time() - runloop->_state.start_time;
        if (runloop->interval > 0 && runloop->_state.is_running)
        {
            runloop->_state.exp_time = runloop->_state.exp_time + runloop->interval;
            if (runloop->_state.exp_time - runloop->_state.running_time <= runloop->interval)
            {
                hi_sleep(runloop->_state.exp_time - runloop->_state.running_time);
                runloop->_state.running_time = runloop->_state.exp_time;
            }
        }
    }

    runloop->_state.running_time = hi_get_time() - runloop->_state.start_time;
    if (runloop->end_func) {
        runloop->end_func(runloop);
    }
    // hi_isr_queue_deinit(&runloop->events);
    hi_mutex_unlock(&(runloop->_state.thread_mutex));
    hi_thread_deinit();
    return NULL;
}

void hi_runloop_init(hi_runloop_t *runloop)
{
    runloop->_state = (hi_runloop_state_t){0};
    hi_mutex_init(&(runloop->_state.thread_mutex));
    runloop->thread.args.byte = runloop;
}

inline void hi_runloop_start(hi_runloop_t *runloop)
{
    if (runloop->_state.is_running) {
        return;
    }
    hi_mutex_lock(&(runloop->_state.thread_mutex));
    hi_thread_init(&(runloop->thread));
}

inline void hi_runloop_wait(hi_runloop_t *runloop)
{
    // hi_thread_join(&(runloop->thread));
    hi_mutex_lock(&(runloop->_state.thread_mutex));
    hi_mutex_unlock(&(runloop->_state.thread_mutex));
}

inline void hi_runloop_stop(hi_runloop_t *runloop)
{
    runloop->_state.is_running = 0;
}

inline uint8_t hi_runloop_is_running(hi_runloop_t *runloop)
{
    return runloop->_state.is_running;
}

inline hi_time_t hi_runloop_running_time(hi_runloop_t *runloop)
{
    return runloop->_state.running_time;
}
