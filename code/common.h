#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define SCREEN_W 1920
#define SCREEN_H 1080
#define Max(a, b) ((a) < (b) ? (b) : (a))
#define FONTSIZE 48
#define GLYPH_CAP 128

typedef unsigned int uint;
typedef size_t usize;

union vec2 {
	float E[2];
	struct { float x, y; };
	float& operator[](usize index) { return E[index]; }
	vec2 operator+(vec2 b) { return vec2{.x = this->x + b.x, .y = this->y + b.y}; }
};
union vec4 {
	float E[4];
	struct { float x, y, z, w; };
	struct { float r, g, b, a; };
	float& operator[](usize index) { return E[index]; }
};

char* load_file(const char* filepath);

#endif // COMMON_H
