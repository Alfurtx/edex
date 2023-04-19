#include "common.h"
#include "renderer.h"
#include "glyph.h"

const char* testline = "HOLA NUEVO MUNDO";

FT_Library ftlibrary;
FT_Face ftface;
GLFWwindow* window;
const char* fontpath = "w:\\edex\\fonts\\liberation_mono.ttf";
static Renderer renderer = {};
static GlyphAtlas ga = {};

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
	window = glfwCreateWindow(SCREEN_W, SCREEN_H, "EDEX", 0, 0);
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

	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		{
			vec4 b = {.r=0, .g=0, .b=1, .a=1};
			vec4 r = {.r=1, .g=0, .b=0, .a=1};
			vec4 g = {.r=0, .g=1, .b=0, .a=1};
			renderer_set_shader(&renderer, SHADER_COLOR);
			renderer_push_rect(&renderer, vec2{.x=0, .y = 0}, vec2{100, 100}, r);
			renderer_push_rect(&renderer, vec2{.x=0, .y = 100}, vec2{100, 100}, b);
			renderer_flush(&renderer);

			renderer_set_shader(&renderer, SHADER_TEXT);
			glyph_atlas_render_line(&ga, &renderer, testline, strlen(testline), vec2{0, 0}, g);
			renderer_flush(&renderer);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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
