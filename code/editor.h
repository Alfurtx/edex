#ifndef EDITOR_H
#define EDITOR_H

#include "common.h"
#include "glyph.h"
#include "renderer.h"
#include "config.h"

#define CURSOR_OFFSET 0.16f

struct Line {
	usize begin;
	usize end;
};

struct Lines {
	Line* items;
	usize cap;
	usize count;
};

struct Editor {
	GlyphAtlas* atlas;
	GLFWwindow* window;
	char* filepath;

	Buffer data;
	Lines lines;
	usize cursor;
};

void editor_init(Editor* e, GLFWwindow* wnd, GlyphAtlas* ga);
void editor_render(Editor* e, Renderer* r);
void editor_cleanup(Editor* e);
void editor_load_file(Editor* e, const char* filepath);

void editor_move_cursor_right(Editor* e);
void editor_move_cursor_left(Editor* e);

#endif // EDITOR_H
