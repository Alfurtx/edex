#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

#if defined(_WIN32)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <windows.h>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define SCREEN_W 1920
#define SCREEN_H 1080
#define Max(a, b) ((a) < (b) ? (b) : (a))
#define DEFAULT_CAP 512
#define KB(v) (1024LL * v)
#define MB(v) (1024LL * KB(v))
#define GB(v) (1024LL * MB(v))

#define GLYPH_SET 32 // set at 32 cause thats the value unicode non-special characters start
#define GLYPH_CAP 128

typedef uint8_t u8;
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

struct Buffer {
	char* items;
	usize cap;
	usize count;
};

char* load_file(const char* filepath);

#define buffer_append(buf, item)					\
	do {								\
		if((buf)->count >= (buf)->cap) {			\
			(buf)->cap = (buf)->cap == 0 ? DEFAULT_CAP : (buf)->cap*2; \
			(buf)->items = (Line*)realloc((buf)->items, (buf)->cap * sizeof(*(buf)->items)); \
			assert((buf)->items && "Realloc Failed");	\
		}							\
		(buf)->items[(buf)->count++] = (item);			\
	} while(0)
#define buffer_append_many(buf, mitems, icount)				\
	do {								\
		if((buf)->count + icount > (buf)->cap) {		\
			if((buf)->cap == 0) {				\
				(buf)->cap = DEFAULT_CAP;		\
			}						\
			while((buf)->count + icount >= (buf)->cap) {	\
				(buf)->cap *= 2;			\
			}						\
			(buf)->items = (char*)realloc((buf)->items, (buf)->cap * sizeof(*(buf)->items)); \
			assert((buf)->items && "Realloc Failed");	\
		}							\
		memcpy((buf)->items + (buf)->count, mitems, icount * sizeof(*(buf)->items)); \
		(buf)->count += icount;					\
	} while(0)
#define buffer_append_cstr(buf, _str)		\
	do {					\
		const char* _s = (_str);	\
		size_t n = strlen(_s);		\
		buffer_append_many(buf, _s, n);	\
	} while(0)
#define buffer_append_null(buf) buffer_append_many(buf, "", 1)



#endif // COMMON_H
