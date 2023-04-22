#ifndef GLYPH_H
#define GLYPH_H

#include "common.h"
#include "renderer.h"

struct GlyphInfo {
	float ax, ay; // advance (x,y)
	float bw; // bitmap.width
	float bh; // bitmap.rows (aka. height)
	float bl; // bitmap_left
	float bt; // bitmap_top
	float tx; // texture_x_pos
};

struct GlyphAtlas {
	uint width;
	uint height;
	uint texid;
	GlyphInfo characters[GLYPH_CAP];

	/* font info */
	uint line_spacing;
	long descender;
	long glyph_height;
};

void glyph_atlas_init(GlyphAtlas* ga, FT_Face ftface);
void glyph_atlas_render_line(GlyphAtlas* a, Renderer* r, const char* text, usize text_size, vec2 pos, vec4 color);

#endif // GLYPH_H
