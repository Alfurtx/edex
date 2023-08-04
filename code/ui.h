#ifndef UI_H
#define UI_H

#include "common.h"
#include "editor.h"
#include "config.h"

struct UI {
	Editor* editor;
	Renderer* renderer;
};

void ui_init(UI* ui, Editor* e, Renderer* r);
void ui_update(UI* ui);
void ui_render(UI* ui);

#endif
