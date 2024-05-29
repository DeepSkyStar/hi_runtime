/**
 * @file hi_color.c
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

#include "hi_sys.h"
#include "hi_color.h"
#include "hi_math.h"

inline void hi_color_init_rgb(hi_color_t *color, double r, double g, double b)
{
	color->space = HI_COLOR_SPACE_RGB;
	color->value.rgb.r = r;
	color->value.rgb.g = g;
	color->value.rgb.b = b;
}

inline void hi_color_init_hsl(hi_color_t *color, double h, double s, double l)
{
	color->space = HI_COLOR_SPACE_HSL;
	color->value.hsl.h = h;
	color->value.hsl.s = s;
	color->value.hsl.l = l;
}

inline hi_color_t hi_color_rgb2hsl(const hi_color_t *color)
{   
    hi_color_t output_color = {
        .space = HI_COLOR_SPACE_HSL,
        .value = {
            .hsl = {0},
        },
    };
	double max_val, min_val, delta_val = 0.0;

	max_val = MAX(color->value.rgb.r, MAX(color->value.rgb.g, color->value.rgb.b));
	min_val = MIN(color->value.rgb.r, MIN(color->value.rgb.g, color->value.rgb.b));
	delta_val = max_val - min_val;

	output_color.value.hsl.l = (max_val + min_val) / 2.0;

	if (HI_IS_EQUAL(max_val, min_val))
	{
		output_color.value.hsl.h = 0.0;
		output_color.value.hsl.s = 0.0;
	}
	else
	{
		if (HI_IS_EQUAL(color->value.rgb.r, max_val))
			output_color.value.hsl.h = (color->value.rgb.g - color->value.rgb.b) / delta_val / 6.0;
		else if(HI_IS_EQUAL(color->value.rgb.g, max_val))
			output_color.value.hsl.h = (color->value.rgb.b - color->value.rgb.r) / delta_val / 6.0 + 1.0 / 3.0;
		else if (HI_IS_EQUAL(color->value.rgb.b, max_val))
			output_color.value.hsl.h = (color->value.rgb.r - color->value.rgb.g) / delta_val / 6.0 + 2.0 / 3.0;

		if (output_color.value.hsl.h < 0)
			output_color.value.hsl.h += 1.0;

		if (output_color.value.hsl.l > 0.5)
			output_color.value.hsl.s = delta_val / (2.0 - max_val - min_val);
		else
			output_color.value.hsl.s = delta_val / (max_val + min_val);

        output_color.value.hsl.h = output_color.value.hsl.h * 360.0;
	}
    return output_color;
}

double hi_hue2rgb(double min_val, double max_val, double hue)
{
	if (hue < 0.0) hue += 1.0;
	if (hue > 1.0) hue -= 1.0;
	if (6.0 * hue < 1.0) return min_val + (max_val - min_val) * 6.0 * hue;
	if (2.0 * hue < 1.0) return max_val;
	if (3.0 * hue < 2.0) return min_val + (max_val - min_val) * ((2.0 / 3.0) - hue) * 6.0;
	return (min_val);
}

inline hi_color_t hi_color_hsl2rgb(const hi_color_t *color)
{
	double max_val, min_val = 0.0;
    hi_color_t output_color = {
        .space = HI_COLOR_SPACE_RGB,
        .value = {
            .rgb = {0},
        },
    };

	if (HI_IS_ZERO(color->value.hsl.s))
	{
        output_color.value.rgb.r = color->value.hsl.l;
		output_color.value.rgb.g = color->value.hsl.l;
        output_color.value.rgb.b = color->value.hsl.l;
	}
	else
	{
		if (color->value.hsl.l <= 0.5)
			max_val = color->value.hsl.l * (1 + color->value.hsl.s);
		else
			max_val = color->value.hsl.l + color->value.hsl.s - color->value.hsl.l * color->value.hsl.s;
		min_val = 2 * color->value.hsl.l - max_val;

		output_color.value.rgb.r = hi_hue2rgb(min_val, max_val, color->value.hsl.h / 360.0 + (1.0 / 3.0));
		output_color.value.rgb.g = hi_hue2rgb(min_val, max_val, color->value.hsl.h / 360.0);
		output_color.value.rgb.b = hi_hue2rgb(min_val, max_val, color->value.hsl.h / 360.0 - (1.0 / 3.0));
	}
    if (output_color.value.rgb.r < 0.0) output_color.value.rgb.r = 0.0;
    if (output_color.value.rgb.r > 1.0) output_color.value.rgb.r = 1.0;
    if (output_color.value.rgb.g < 0.0) output_color.value.rgb.g = 0.0;
    if (output_color.value.rgb.g > 1.0) output_color.value.rgb.g = 1.0;
    if (output_color.value.rgb.b < 0.0) output_color.value.rgb.b = 0.0;
    if (output_color.value.rgb.b > 1.0) output_color.value.rgb.b = 1.0;
    return output_color;
}
