#include "editor.h"

static void
rebuild_lines(Editor* e)
{
	e->lines.count = 0;
	Line line;
	line.begin = 0;
	for(usize i = 0; i < e->data.count; i++) {
		if(e->data.items[i] == '\n') {
			line.end = i;
			buffer_append(&e->lines, line);
			line.begin = i + 1;
		}
	}
	line.end = e->data.count;
	buffer_append(&e->lines, line);
}

static usize
cursor_get_row(Editor* e)
{
	usize l = 0;
	for(uint i = 0; i < e->lines.count; i++) {
		if(e->cursor >= e->lines.items[i].begin && e->cursor <= e->lines.items[i].end) {
			return(i);
		}
	}

	return(e->lines.count - 1);
}

static float
cursor_get_pos(Editor* e)
{
	float result = 0.0f;
	usize row = cursor_get_row(e);
	Line l = e->lines.items[row];

	usize col = e->cursor - l.begin;
	usize size = l.end - l.begin;

	for(uint i = 0; i < size; i++) {
		if(i == col) return(result);
		usize c = e->data.items[l.begin + i];
		if(c >= GLYPH_CAP) c = '?';
		GlyphInfo g = e->atlas->characters[c];
		result += g.ax;
	}

	return(result);
}

static float
cursor_get_width(Editor* e) {
	float w = 0.0f;
	usize row = cursor_get_row(e);
	Line l = e->lines.items[row];
	usize col = e->cursor - l.begin;
	usize size = l.end - l.begin;

	for(uint i = 0; i < size; i++) {
		usize c = e->data.items[l.begin + i];
		if(c >= GLYPH_CAP) c = '?';
		GlyphInfo g = e->atlas->characters[c];
		w = g.ax;
		if(i == col) return(w);
	}
	return(w);
}

static void
_print_cursor_info(Editor* e)
{
	int w,h; glfwGetWindowSize(e->window, &w, &h);
	vec2 cursorpos = {};
	float cursorwidth = cursor_get_width(e) == 0.0f ? e->atlas->characters['A'].ax : cursor_get_width(e);
	long cursorheight = e->atlas->glyph_height + (e->atlas->line_spacing - e->atlas->glyph_height);
	long ch_offset = e->atlas->descender;
	{
		usize row = cursor_get_row(e);
		cursorpos.x = (-w / 2.0f) + cursor_get_pos(e);
		cursorpos.y = (float) -((-h/2.0f) + (row + 1) * e->atlas->line_spacing);
		cursorpos.y -= ch_offset;
		cursorpos.y -= (float)(e->atlas->line_spacing - e->atlas->glyph_height) / 2;
	}

	if(e->data.items[e->cursor] == '\r') {
		printf("LETRA: \\r | %d |", e->data.items[e->cursor]);
	} else if(e->data.items[e->cursor] == '\n'){
		printf("LETRA: \\n | %d |", e->data.items[e->cursor]);
	} else {
		printf("LETRA: %c | %d |", e->data.items[e->cursor], e->data.items[e->cursor]);
	}
	printf(" CURSOR: %zu |", e->cursor);
	printf(" POSITION: x = %f , y = %f |", cursorpos.x, cursorpos.y);
	printf("\n");
}


void
editor_init(Editor* e, GLFWwindow* wnd, GlyphAtlas* ga, Arena* a)
{
	e->atlas = ga;
	e->window = wnd;
	e->filepath = 0;
	e->arena = a;

	int w,h; glfwGetWindowSize(wnd, &w, &h);
	e->camera_limits = {
		.x = (float)w/2.0f, .y = (float)-w/2.0f,
		.z = (float)h/2.0f, .w = (float)-h/2.0f
	};

	// set camera_limits taking into account the UI
	// e->camera_limits.w += modeline_height;
}

void
editor_load_file(Editor* e, const char* filepath)
{
	e->data.count = 0;
	e->filepath = (char*)filepath;
	char* file = load_file(filepath);
	buffer_append_cstr(&e->data, file);
	free(file);
	rebuild_lines(e);
}

void
editor_render(Editor* e, Renderer* r)
{
	int w,h; glfwGetWindowSize(e->window, &w, &h);
	r->resolution = vec2{(float)w,(float)h};
	r->time = (float)glfwGetTime();

	/* Cursor info */
	vec2 cursorpos = {};
	float cursorwidth = cursor_get_width(e) == 0.0f ? e->atlas->characters['A'].ax : cursor_get_width(e);
	long cursorheight = e->atlas->glyph_height + (e->atlas->line_spacing - e->atlas->glyph_height);
	long ch_offset = e->atlas->descender;
	{
		usize row = cursor_get_row(e);
		cursorpos.x = (-w / 2.0f) + cursor_get_pos(e);
		cursorpos.y = (float) -((-h/2.0f) + (row + 1) * e->atlas->line_spacing);
		cursorpos.y -= ch_offset;
		cursorpos.y -= (float)(e->atlas->line_spacing - e->atlas->glyph_height) / 2;
	}

	/* Cursor renderer */
	{
		renderer_set_shader(r, SHADER_COLOR);
		renderer_push_rect(r, cursorpos, vec2{cursorwidth, (float)cursorheight}, cursor_color);
		renderer_flush(r);
	}

	/* Text renderer */
	{
		renderer_set_shader(r, SHADER_TEXT);
		for(usize i = 0; i < e->lines.count; i++) {
			vec2 t = vec2{
				-(w / 2.0f),
				// -(h / 2.0f) + (e->lines.count - i) * e->atlas->line_spacing
				(h / 2.0f) - (i + 1) * e->atlas->line_spacing
			};
			Line l = e->lines.items[i];
			glyph_atlas_render_line(e->atlas,
						r,
						&e->data.items[l.begin],
						l.end - l.begin,
						t,
						default_text_color);
		}
		renderer_flush(r);
	}

	/* Camera Update */
	{
		if(cursorpos.y < e->camera_limits.w) {
			r->camera_pos.y -= e->atlas->line_spacing;
			e->camera_limits.z -= e->atlas->line_spacing;
			e->camera_limits.w -= e->atlas->line_spacing;
		} else if(cursorpos.y + (float)cursorheight > e->camera_limits.z) {
			r->camera_pos.y += e->atlas->line_spacing;
			e->camera_limits.z += e->atlas->line_spacing;
			e->camera_limits.w += e->atlas->line_spacing;
		}
	}
}

void
editor_cleanup(Editor* e)
{
	if(e->data.items != 0) {
		free(e->data.items);
	}
	if(e->lines.items != 0) {
		free(e->lines.items);
	}
}

void
editor_move_cursor_right(Editor* e)
{
	if(e->data.items[e->cursor] == eol_character[0]) e->cursor += eol_offset;
	else if(e->cursor < e->data.count) e->cursor += 1;
	// _print_cursor_info(e);
}

void
editor_move_cursor_left(Editor* e)
{
	if(e->data.items[e->cursor] == eol_character[0]) e->cursor -= eol_offset;
	else if(e->cursor > 0) e->cursor -= 1;
	// _print_cursor_info(e);
}

void
editor_move_cursor_up(Editor* e)
{
	usize row = cursor_get_row(e);
	if(row > 0) {
		Line line = e->lines.items[row];
		usize diff = e->cursor - line.begin;
		Line nline = e->lines.items[row - 1];
		if(nline.begin + diff < nline.end) {
			e->cursor = nline.begin + diff;
		} else {
			e->cursor = nline.end;
		}
	}
}

void
editor_move_cursor_down(Editor* e)
{
	usize row = cursor_get_row(e);
	if(row < e->lines.count - 1) {
		Line l = e->lines.items[row];
		usize diff = e->cursor - l.begin;
		Line nl = e->lines.items[row + 1];
		if(nl.begin + diff < nl.end) {
			e->cursor = nl.begin + diff;
		} else {
			e->cursor = nl.end;
		}
	}
}
