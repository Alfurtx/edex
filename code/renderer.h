#ifndef RENDERER_H
#define RENDERER_H

#include "common.h"
#define VERTCAP (3 * 640 * 1000)

struct Vertex {
	vec2 pos;
	vec2 uv;
	vec4 color;
};
enum UniformSlot {
	UNIFORM_TIME = 0,
	UNIFORM_RESOLUTION,
	UNIFORM_CAMERA_POS,
	UNIFORM_COUNT
};
enum Shader {
	SHADER_COLOR = 0,
	SHADER_TEXT,
	SHADER_COUNT
};
struct Renderer {
	uint vao, vbo;
	uint programs[SHADER_COUNT];
	Shader current_shader;
	int uniforms[UNIFORM_COUNT];
	Vertex verticies[VERTCAP];
	usize verticies_count;

	struct {
		bool polymode;
	} flags;

	vec2 camera_pos;
	vec2 resolution;
	float time;
};

void renderer_init(Renderer* r, GLFWwindow* window);
void renderer_set_shader(Renderer* r, Shader s);
void renderer_flush(Renderer* r);
void renderer_sync(Renderer* r);
void renderer_draw(Renderer* r);

void renderer_push_vertex(Renderer* r, vec2 pos, vec4 col, vec2 uv);
void renderer_push_rect(Renderer* r, vec2 pos, vec2 size, vec4 color);
void renderer_push_image_rect(Renderer* r, vec2 p, vec2 s, vec2 uvp, vec2 uvs, vec4 c);
void renderer_push_triangle(Renderer* r,
			    vec2 p0,  vec2 p1,  vec2 p2,
			    vec4 c0,  vec4 c1,  vec4 c2,
			    vec2 uv0, vec2 uv1, vec2 uv2);
void renderer_push_quad(Renderer* r,
			vec2 p0,  vec2 p1,  vec2 p2,  vec2 p3,
			vec4 c0,  vec4 c1,  vec4 c2,  vec4 c3,
			vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3);

#endif // RENDERER_H
