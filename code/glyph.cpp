#include "glyph.h"

void
glyph_atlas_init(GlyphAtlas* ga, FT_Face ftface)
{
	for(uint i = 32; i < 128; i++) {
		FT_Load_Char(ftface, i, FT_LOAD_RENDER);
		ga->width += ftface->glyph->bitmap.width;
		ga->height = Max(ga->height, ftface->glyph->bitmap.rows);
	}
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &ga->texid);
	glBindTexture(GL_TEXTURE_2D, ga->texid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (usize)ga->width, (usize)ga->height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	uint x = 0;
	for(uint i = 32; i < 128; i++) {
		FT_Load_Char(ftface, i, FT_LOAD_RENDER);
		ga->characters[i] = {
			.ax = (float) (ftface->glyph->advance.x >> 6),
			.ay = (float) (ftface->glyph->advance.y >> 6),
			.bw = (float) ftface->glyph->bitmap.width,
			.bh = (float) ftface->glyph->bitmap.rows,
			.bl = (float) ftface->glyph->bitmap_left,
			.bt = (float) ftface->glyph->bitmap_top,
			.tx = (float) x / (float) ga->width
		};
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, ftface->glyph->bitmap.width, ftface->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, ftface->glyph->bitmap.buffer);
		x += ftface->glyph->bitmap.width;
	}
}

void
glyph_atlas_render_line(GlyphAtlas* a, Renderer* r,
			const char* text, usize text_size,
			vec2 pos, vec4 color)
{
	for(uint i = 0; i < text_size; i++) {
		usize char_index = text[i];
		if(char_index >= GLYPH_CAP) {
			char_index = '?';
		}
		GlyphInfo gi = a->characters[char_index];
		float x = pos.x + gi.bl;
		float y = -pos.y - gi.bt;
		float w = gi.bw;
		float h = gi.bh;
		pos.x += gi.ax;
		pos.y += gi.ay;

		renderer_push_image_rect(r,
					 vec2{x,-y},
					 vec2{w,-h},
					 vec2{gi.tx,0},
					 vec2{w / (float) a->width, h / (float) a->height},
					 color
					 );
	}
}
