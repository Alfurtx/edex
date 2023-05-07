#include "renderer.h"

/*
 * TODO(fonsi): make this independent of path
 */
static const char* vertexpath = "e:\\Home\\Work\\edex\\shaders\\simple.vert";
static const char* shaderpath[SHADER_COUNT] = {
	"e:\\Home\\Work\\edex\\shaders\\color.frag",
	"e:\\Home\\Work\\edex\\shaders\\text.frag",
};
static const char* uniform_names[UNIFORM_COUNT] = {
	"u_time",
	"u_resolution",
	"u_camera_pos",
};


static uint
compile_shader(usize shader_type, const char* filepath)
{
	uint shader_id;
	shader_id = glCreateShader(shader_type);
	char* source = load_file(filepath);
	glShaderSource(shader_id, 1, &source, NULL);
	glCompileShader(shader_id);

	int  success;
	char infolog[512];
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(shader_id, 512, NULL, infolog);
		fprintf(stderr, "%s SHADER COMPILATION FAILED:\nFILE: %s\nLOG: %s\n",
			shader_id == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT",
			filepath,
			infolog);
		free(source);
		exit(1);
	}
	free(source);
	return(shader_id);
}

static void
create_programs(Renderer* r)
{
	uint vert_shader = compile_shader(GL_VERTEX_SHADER, vertexpath);
	for(uint i = 0; i < SHADER_COUNT; i++) {
		uint frag_shader = compile_shader(GL_FRAGMENT_SHADER, shaderpath[i]);
		uint p = glCreateProgram();
		glAttachShader(p, vert_shader);
		glAttachShader(p, frag_shader);
		glLinkProgram(p);

		int  success;
		char infolog[512];
		glGetProgramiv(p, GL_LINK_STATUS, &success);
		if(!success) {
			glGetProgramInfoLog(p, 512, NULL, infolog);
			fprintf(stderr, "PROGRAM LINKING FAILED:\nLOG: %s\n", infolog);
		}
		r->programs[i] = p;
		glDeleteShader(frag_shader);
	}
	glDeleteShader(vert_shader);
}

void
renderer_init(Renderer* r, GLFWwindow* window)
{
	glGenBuffers(1, &r->vbo);
	glGenVertexArrays(1, &r->vao);
	glBindVertexArray(r->vao);
	glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(r->verticies), r->verticies, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0); /* pos */
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
	glEnableVertexAttribArray(1); /* uv */
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(2); /* col */
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float)));

	int w,h; glfwGetWindowSize(window, &w, &h);
	r->verticies_count = 0;
	r->resolution = {(float) w, (float) h};
	r->camera_pos = {};
	r->time = (float)glfwGetTime();
	r->window = window;

	create_programs(r);
}

void
renderer_set_shader(Renderer* r, Shader s)
{
	int w,h; glfwGetWindowSize(r->window, &w, &h);
	r->resolution = {(float) w, (float) h};

	r->current_shader = s;
	glUseProgram(r->programs[s]);
	for(uint i = 0; i < UNIFORM_COUNT; i++) {
		r->uniforms[i] = glGetUniformLocation(r->programs[s], uniform_names[i]);
	}

	glUniform2f(r->uniforms[UNIFORM_RESOLUTION], r->resolution.x, r->resolution.y);
	glUniform2f(r->uniforms[UNIFORM_CAMERA_POS], r->camera_pos.x, r->camera_pos.y);
	glUniform1f(r->uniforms[UNIFORM_TIME], r->time);
}

void
renderer_flush(Renderer* r)
{
	renderer_sync(r);
	renderer_draw(r);
	r->verticies_count = 0;
}

void
renderer_sync(Renderer* r)
{
	glBufferSubData(GL_ARRAY_BUFFER, 0, r->verticies_count * sizeof(Vertex), r->verticies);
}

void
renderer_draw(Renderer* r)
{
	glPolygonMode(GL_FRONT_AND_BACK, r->flags.polymode ? GL_LINE : GL_FILL);
	glDrawArrays(GL_TRIANGLES, 0, r->verticies_count);
}

void
renderer_push_vertex(Renderer* r, vec2 pos, vec4 col, vec2 uv)
{
	r->verticies[r->verticies_count] = {
		.pos = pos,
		.uv = uv,
		.color = col,
	};
	r->verticies_count += 1;
}

void
renderer_push_rect(Renderer* r, vec2 pos, vec2 size, vec4 color)
{
	vec2 uv = {0};
	renderer_push_quad(r,
			   pos,
			   pos + vec2{.x = size.x, .y = 0},
			   pos + vec2{.x = 0, .y = size.y},
			   pos + size,
			   color,color,color,color,
			   uv,uv,uv,uv);
}

void
renderer_push_image_rect(Renderer* r, vec2 p, vec2 s, vec2 uvp, vec2 uvs, vec4 c)
{
	renderer_push_quad(r,
			   p,
			   vec2{.x = p.x + s.x, .y = p.y},
			   vec2{.x = p.x,       .y = p.y + s.y},
			   vec2{.x = p.x + s.x, .y = p.y + s.y},
			   c,c,c,c,
			   uvp,
			   vec2{.x = uvp.x + uvs.x, .y = uvp.y},
			   vec2{.x = uvp.x,         .y = uvp.y + uvs.y},
			   uvp + uvs);
}

void
renderer_push_triangle(Renderer* r,
		       vec2 p0,  vec2 p1,  vec2 p2,
		       vec4 c0,  vec4 c1,  vec4 c2,
		       vec2 uv0, vec2 uv1, vec2 uv2)
{
	renderer_push_vertex(r, p0, c0, uv0);
	renderer_push_vertex(r, p1, c1, uv1);
	renderer_push_vertex(r, p2, c2, uv2);
}

void
renderer_push_quad(Renderer* r,
		   vec2 p0,  vec2 p1,  vec2 p2,  vec2 p3,
		   vec4 c0,  vec4 c1,  vec4 c2,  vec4 c3,
		   vec2 uv0, vec2 uv1, vec2 uv2, vec2 uv3)
{
	renderer_push_triangle(r, p0, p1, p2, c0, c1, c2, uv0, uv1, uv2);
	renderer_push_triangle(r, p1, p2, p3, c1, c2, c3, uv1, uv2, uv3);
}
