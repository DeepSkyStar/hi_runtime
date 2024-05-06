/**
 * @file hi_filter.c
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

#include "hi_filter.h"
#include "hi_math.h"

void hi_smooth_filter_init(hi_smooth_filter_t *filter, uint32_t smooth_time)
{
    filter->smooth_time = smooth_time;
}

void hi_smooth_filter_set_target(hi_smooth_filter_t *filter, double target, uint32_t time)
{
    if (HI_IS_EQUAL(filter->target_val, target)) return;
    filter->last_val = filter->smooth_val;
    filter->target_val = target;
    filter->target_time = time + filter->smooth_val;
}

void hi_smooth_filter_update(hi_smooth_filter_t *filter, uint32_t time)
{
    if (filter->target_time <= time) 
    {
        filter->smooth_val = filter->target_val;
        return;
    }
    filter->smooth_val = filter->last_val + (filter->target_val - filter->last_val) * (filter->target_time - time) / filter->smooth_time;
}

void hi_rgb_color_filter_init(hi_rgb_color_filter_t *filter, uint32_t smooth_time)
{
    filter->r.smooth_time = smooth_time;
    filter->g.smooth_time = smooth_time;
    filter->b.smooth_time = smooth_time;
}

void hi_rgb_color_filter_set_target(hi_rgb_color_filter_t *filter, hi_color_t *color, uint32_t time)
{
    if (color->space != HI_COLOR_SPACE_RGB) return;
    hi_smooth_filter_set_target(&(filter->r), color->value.rgb.r, time);
    hi_smooth_filter_set_target(&(filter->g), color->value.rgb.g, time);
    hi_smooth_filter_set_target(&(filter->b), color->value.rgb.b, time);
}

void hi_rgb_color_filter_update(hi_rgb_color_filter_t *filter, uint32_t time)
{
    hi_smooth_filter_update(&(filter->r), time);
    hi_smooth_filter_update(&(filter->g), time);
    hi_smooth_filter_update(&(filter->b), time);
}

void hi_delay_filter_init(hi_delay_filter_t *filter, uint32_t delay_time)
{
    filter->last_val = NULL;
    filter->delay_time = delay_time;
    filter->last_time = 0;
}

void hi_delay_filter_set(hi_delay_filter_t *filter, void *last_val, uint32_t time)
{
    filter->last_val = last_val;
    filter->last_time = time;
}

void* hi_delay_filter_get(hi_delay_filter_t *filter, uint32_t time)
{
    void* result = NULL;
    if (filter->last_time + filter->delay_time <= time)
    {
        result = filter->last_val;
        filter->last_val = NULL;
    }
    return result;
}

void hi_attenuator_init(hi_attenuator_t *filter, double factor)
{
    filter->factor = factor;
}

double hi_attenuator_damping(hi_attenuator_t *filter, double value)
{
    if (HI_IS_ZERO(filter->factor)) return value;
    return value / filter->factor;
}