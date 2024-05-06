/**
 * @file hi_color.h
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

#ifndef HI_COLOR_H_
#define HI_COLOR_H_

#include "hi_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t hi_color_space_t;

enum hi_color_space_enum{
    HI_COLOR_SPACE_NONE = 0,
    HI_COLOR_SPACE_RGB,
    HI_COLOR_SPACE_HSL,
};

typedef struct hi_color_s
{
    hi_color_space_t space;
    union
    {
        struct
        {
            // 0.0~1.0.
            double r;
            // 0.0~1.0.
            double g;
            // 0.0~1.0.
            double b;
        } rgb;
        struct
        {
            // 0.0~360.0.
            double h;
            // 0.0~1.0.
            double s;
            // 0.0~1.0.
            double l;
        } hsl;
    }value;
}hi_color_t;

extern void hi_color_init_rgb(hi_color_t *color, double r, double g, double b);
extern void hi_color_init_hsl(hi_color_t *color, double h, double s, double l);

extern hi_color_t hi_color_rgb2hsl(const hi_color_t *color);
extern hi_color_t hi_color_hsl2rgb(const hi_color_t *color);

#ifdef __cplusplus
}
#endif

#endif
