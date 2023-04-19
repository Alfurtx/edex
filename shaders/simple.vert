#version 330 core

layout (location = 0) in vec2 pos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec4 col;

uniform vec2 u_resolution;
uniform vec2 u_camera_pos;
uniform vec4 u_time;

out vec4 out_color;
out vec2 out_uv;

mat4
ortho_mat(void) {
  float l = 0.0f; // left
  float r = u_resolution.x; // right
  float b = 0.0f; // bottom
  float t = u_resolution.y; // top
  float n = 0.0f; // near
  float f = 4000.0f; // far

  mat4 projection = mat4(vec4(2.0/(r-l),     0.0,          0.0,         0.0),
			 vec4(0.0,           2.0/(t-b),    0.0,         0.0),
			 vec4(0.0,           0.0,         -2.0/(f-n),   0.0),
			 vec4(-(r+l)/(r-l), -(t+b)/(t-b), -(f+n)/(f-n), 1.0));

  return projection;
}

vec2
camera_project(vec2 point)
{
  float camera_scale = 1.0f;
  return 2.0f * (point - u_camera_pos) / u_resolution;
}

void
main(void)
{
  out_color = col;
  out_uv = uv;
  gl_Position = ortho_mat() * vec4(pos.xy, 0.0, 1.0);
  // gl_Position = vec4(camera_project(pos),0,1);
}
