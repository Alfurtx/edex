#ifndef GLYPH_H
#define GLYPH_H

#include "common.h"
#include "renderer.h"

struct GlyphInfo {
	float ax, ay;
	float bw;
	float bh;
	float bl;
	float bt;
	float tx;
};

struct GlyphAtlas {
	uint width;
	uint height;
	uint texid;
	GlyphInfo characters[GLYPH_CAP];
};

void glyph_atlas_init(GlyphAtlas* ga, FT_Face ftface);
void glyph_atlas_render_line(GlyphAtlas* a, Renderer* r, const char* text, usize text_size, vec2 pos, vec4 color);

#endif // GLYPH_H
