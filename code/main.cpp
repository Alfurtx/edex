#include "common.h"
#include "config.h"
#include "renderer.h"
#include "glyph.h"
#include "editor.h"

/*
 * TODO:
 * -> Colorscheme stuff:
 * How to separate foreground color from background color (eg. have a bg and fg for the cursor)
 * -> Change the text + cursor rendering position calculations
 * The text and cursor are positioned based on the screen coordinates which is insanly bad
 * because it doesn't allow for good "camera" scrolling as well as having text be outside the
 * cursor when you have it hover over a character, etc.
 */

const char* testline = "HOLA NUEVO MUNDO";
const char* fontpath = "w:\\edex\\fonts\\liberation_mono.ttf";
const char* testpath = "w:\\edex\\test.txt";
const char* test2path = "w:\\edex\\test2.txt";

FT_Library ftlibrary;
FT_Face ftface;
GLFWwindow* window;

static Renderer renderer = {};
static GlyphAtlas ga = {};
static Editor e = {};

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

	FT_Init_FreeType(&ftlibrary);

	if(FT_New_Face(ftlibrary, fontpath, 0, &ftface)) {
		fprintf(stderr, "FT_Face NOT initalized\n");
		exit(1);
	}

	FT_Set_Pixel_Sizes(ftface, 0, FONTSIZE);

	glyph_atlas_init(&ga, ftface);
	renderer_init(&renderer, window);
	editor_init(&e, window, &ga);
	editor_load_file(&e, test2path);

	while(!glfwWindowShouldClose(window)) {
		glClearColor(default_bg_color.r,
			     default_bg_color.g,
			     default_bg_color.b,
			     default_bg_color.a);
		glClear(GL_COLOR_BUFFER_BIT);

		editor_render(&e, &renderer);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	editor_cleanup(&e);
	FT_Done_Face(ftface);
	FT_Done_FreeType(ftlibrary);
        glfwTerminate();
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
