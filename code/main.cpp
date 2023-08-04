#include "common.h"
#include "config.h"
#include "renderer.h"
#include "glyph.h"
#include "editor.h"
#include "ui.h"

/*
 * TODO:
 * -> Colorscheme stuff:
 * How to separate foreground color from background color (eg. have a bg and fg for the cursor)
 */

const char* testline = "HOLA NUEVO MUNDO";
const char* fontpath =  "..\\fonts\\liberation_mono.ttf";
// const char* fontpath =  "e:\\Home\\Work\\edex\\fonts\\liberation_mono.ttf";
const char* testpath =  "e:\\Home\\Work\\edex\\test.txt";
// const char* test2path = "e:\\Home\\Work\\edex\\test2.txt";
const char* test2path = "..\\test2.txt";

FT_Library ftlibrary;
FT_Face ftface;
GLFWwindow* window;

static Renderer renderer = {};
static GlyphAtlas ga = {};
static Editor e = {};
static Arena arena = {};
static UI ui = {};

void message_debug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
void resize_window(GLFWwindow* wnd, int w, int h);
void process_key_input(GLFWwindow* wnd, int key, int scancode, int action, int mods);

int
main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(SCREEN_W, SCREEN_H, "edex", 0, 0);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, resize_window);
	glfwSetKeyCallback(window, process_key_input);
	glfwSwapInterval(1);
	gladLoadGL();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if(GLAD_GL_ARB_debug_output) {
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(message_debug, 0);
	} else {
		fprintf(stderr, "WARNING: GLAD_GL_ARB_debug_output is not available");
	}

	char exe_path[512];
	{ // trim the executable name out of the path and set it to a variable for relative path file loading
		uint s = GetModuleFileNameA(0, exe_path, 512);
		uint exe_name_size = 8; // e d e x . e x e
	        memset(&exe_path[s - exe_name_size], 0, exe_name_size);
	}

	char font_load_path[512];
	memcpy(font_load_path, exe_path, 512);
	strcat(font_load_path, fontpath);

	FT_Init_FreeType(&ftlibrary);

	if(FT_New_Face(ftlibrary, font_load_path, 0, &ftface)) {
		fprintf(stderr, "FT_Face NOT initalized\n");
		exit(1);
	}

	FT_Set_Pixel_Sizes(ftface, 0, FONTSIZE);

	glyph_atlas_init(&ga, ftface);
	renderer_init(&renderer, window);
	editor_init(&e, window, &ga, &arena);
	ui_init(&ui, &e, &renderer);

	char testfile_rel_path[512];
	memcpy(testfile_rel_path, exe_path, 512);
	strcat(testfile_rel_path, test2path);
	editor_load_file(&e, testfile_rel_path);

	while(!glfwWindowShouldClose(window)) {
		glClearColor(default_bg_color.r,
			     default_bg_color.g,
			     default_bg_color.b,
			     default_bg_color.a);
		glClear(GL_COLOR_BUFFER_BIT);

		editor_render(&e, &renderer);
		ui_render(&ui);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	editor_cleanup(&e);
	FT_Done_Face(ftface);
	FT_Done_FreeType(ftlibrary);
        glfwTerminate();

	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
	_CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
	_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDOUT);
	_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDOUT);
	_CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDOUT);

	_CrtDumpMemoryLeaks();

	return(0);
}

void
resize_window(GLFWwindow* wnd, int w, int h)
{
	glViewport(0,0,w,h);
}

void
process_key_input(GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(wnd, 1);
	}
	if(key == GLFW_KEY_TAB && action == GLFW_PRESS) {
		renderer.flags.polymode = !renderer.flags.polymode;
	}
	if(key == GLFW_KEY_L && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		editor_move_cursor_right(&e);
	}
	if(key == GLFW_KEY_H && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		editor_move_cursor_left(&e);
	}
	if(key == GLFW_KEY_J && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		editor_move_cursor_down(&e);
	}
	if(key == GLFW_KEY_K && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
		editor_move_cursor_up(&e);
	}
}

void
message_debug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    (void)source;
    (void)id;
    (void)length;
    (void)userParam;

    fprintf(stderr,
            "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type,
            severity,
            message);
}
