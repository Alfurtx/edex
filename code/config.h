#ifndef CONFIG_H
#define CONFIG_H

#include "common.h"

static vec4 red{ .r = 1.0f, .g = 0.0f, .b = 0.0f, .a = 1.0f };
static vec4 green{ .r = 0.0f, .g = 1.0f, .b = 0.0f, .a = 1.0f };
static vec4 blue{ .r = 0.0f, .g = 0.0f, .b = 1.0f, .a = 1.0f };

// HANDMADE HERO Colorscheme colors
static vec4 default_text_color{.r = 160.0f / 255.0f, .g = 133.0f / 255.0f, .b = 99.0f / 255.0f, .a = 1.0f};
static vec4 default_bg_color{.r = 22.0f / 255.0f, .g = 22.0f / 255.0f, .b = 22.0f / 255.0f, .a = 1.0f};
static vec4 cursor_color{.r = 64.0f / 255.0f, .g = 255.0f / 255.0f, .b = 64.0f / 255.0f, .a = 1.0f};

#endif
