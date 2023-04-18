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

GlyphAtlas ca;
FT_Library ftlibrary;
FT_Face ftface;
GLFWwindow* window;
const char* fontpath = "w:\\edex\\fonts\\liberation_mono.ttf";
static Renderer renderer = {0};

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

	/* Character Atlas Init */
	{
		for(uint i = 32; i < 128; i++) {
			FT_Load_Char(ftface, i, FT_LOAD_RENDER);
			ca.width += ftface->glyph->bitmap.width;
			ca.height = Max(ca.height, ftface->glyph->bitmap.rows);
		}
		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &ca.texid);
		glBindTexture(GL_TEXTURE_2D, ca.texid);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (usize)ca.width, (usize)ca.height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

		uint x = 0;
		for(uint i = 32; i < 128; i++) {
			FT_Load_Char(ftface, i, FT_LOAD_RENDER);
			ca.characters[i] = {
				.ax = (float) (ftface->glyph->advance.x >> 6),
				.ay = (float) (ftface->glyph->advance.y >> 6),
				.bw = (float) ftface->glyph->bitmap.width,
				.bh = (float) ftface->glyph->bitmap.rows,
				.bl = (float) ftface->glyph->bitmap_left,
				.bt = (float) ftface->glyph->bitmap_top,
				.tx = (float) x / (float) ca.width
			};
			glPixelStorei(GL_UNPACK_ALIGNMENT,1);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, ftface->glyph->bitmap.width, ftface->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, ftface->glyph->bitmap.buffer);
			x += ftface->glyph->bitmap.width;
		}
	}

	renderer_init(&renderer, window);

	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		{
			vec4 b = {.r=0, .g=0, .b=1, .a=1};
			vec4 r = {.r=1, .g=0, .b=0, .a=1};
			renderer_set_shader(&renderer, SHADER_COLOR);
			renderer_push_rect(&renderer, vec2{.x=0, .y = 0}, vec2{100, 100}, r);
			renderer_push_rect(&renderer, vec2{.x=0, .y = 100}, vec2{100, 100}, b);
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
