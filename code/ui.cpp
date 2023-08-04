#include "ui.h"

void
ui_init(UI* ui, Editor* e, Renderer* r)
{
	*ui = { .editor = e, .renderer = r };
}

void
ui_update(UI* ui)
{
}

void
ui_render(UI* ui)
{
	int w,h; glfwGetWindowSize(ui->editor->window, &w, &h);

	// modeline bar?
	vec2 bar_pos = {
		ui->editor->camera_limits.y,
		ui->editor->camera_limits.w
	};
	vec2 bar_size = {
		(float) w,
		(float) modeline_height
	};
	renderer_set_shader(ui->renderer, SHADER_COLOR);
	renderer_push_rect(ui->renderer, bar_pos, bar_size, blue);
	renderer_flush(ui->renderer);

	const char* bartext = "Modeline: Normal 14:79 GIT(main)";
	const usize bartextsize = strlen("Modeline: Normal 14:79 GIT(main)");

	renderer_set_shader(ui->renderer, SHADER_TEXT);

	glyph_atlas_render_line(ui->editor->atlas, ui->renderer,
				bartext,
				bartextsize,
				{bar_pos.x, bar_pos.y},
				green);

	renderer_flush(ui->renderer);
}
