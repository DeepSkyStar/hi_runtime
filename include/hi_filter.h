/**
 * @file hi_filter.h
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

#ifndef HI_FILTER_H_
#define HI_FILTER_H_

#include "hi_defines.h"
#include "hi_color.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hi_smooth_filter_s hi_smooth_filter_t;
typedef struct hi_rgb_color_filter_s hi_rgb_color_filter_t; 
typedef struct hi_delay_filter_s hi_delay_filter_t;
typedef struct hi_attenuator_s hi_attenuator_t;

struct hi_smooth_filter_s
{
    double last_val;
    double target_val;
    double smooth_val;
    uint32_t target_time;   //the time will arrive.
    uint32_t smooth_time;   //how long time to change.
};

void hi_smooth_filter_init(hi_smooth_filter_t *filter, uint32_t smooth_time);
void hi_smooth_filter_set_target(hi_smooth_filter_t *filter, double target, uint32_t time);
void hi_smooth_filter_update(hi_smooth_filter_t *filter, uint32_t time);


struct hi_rgb_color_filter_s
{
    hi_smooth_filter_t r;
    hi_smooth_filter_t g;
    hi_smooth_filter_t b;
};

void hi_rgb_color_filter_init(hi_rgb_color_filter_t *filter, uint32_t smooth_time);
void hi_rgb_color_filter_set_target(hi_rgb_color_filter_t *filter, hi_color_t *color, uint32_t time);
void hi_rgb_color_filter_update(hi_rgb_color_filter_t *filter, uint32_t time);


struct hi_delay_filter_s
{
    void *last_val;
    uint32_t last_time;
    uint32_t delay_time;
};

void hi_delay_filter_init(hi_delay_filter_t *filter, uint32_t delay_time);
void hi_delay_filter_set(hi_delay_filter_t *filter, void *current, uint32_t time);
void* hi_delay_filter_get(hi_delay_filter_t *filter, uint32_t time);

struct hi_attenuator_s
{
    double factor; //0.0 is no change.
};

void hi_attenuator_init(hi_attenuator_t *filter, double factor);
double hi_attenuator_damping(hi_attenuator_t *filter, double value);

#ifdef __cplusplus
}
#endif

#endif