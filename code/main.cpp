#include <stdio.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define SCREEN_W 1920
#define SCREEN_H 1080
#define Max(a, b) ((a) < (b) ? (b) : (a))
#define FONTSIZE 48

typedef unsigned int uint;
typedef size_t usize;

union vec2 {
	float E[2];
	struct { float x, y; };
	float& operator[](usize index) { return E[index]; }
};

struct GlypInfo {

};

struct GlyphAtlas {
	uint width;
	uint height;
	uint texid;
};

GlyphAtlas ca;
FT_Library ftlibrary;
FT_Face ftface;
GLFWwindow* window;
const char* fontpath = "w:\\edex\\fonts\\liberation_mono.ttf";

void ResizeWindow(GLFWwindow* wnd, int w, int h);

int
main(void)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(SCREEN_W, SCREEN_H, "EDEX", 0, 0);
	glfwMakeContextCurrent(window);
	glfwSetWindowSizeCallback(window, ResizeWindow);
	gladLoadGL();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	FT_Init_FreeType(&ftlibrary);

	if(FT_New_Face(ftlibrary, fontpath, 0, &ftface)) {
		fprintf(stderr, "FT_Face NOT initalized");
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
		}
	}

	while(!glfwWindowShouldClose(window)) {
		glClearColor(0.5f, 0.8f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		if(glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
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
ResizeWindow(GLFWwindow* wnd, int w, int h)
{
	glViewport(0,0,w,h);
}
