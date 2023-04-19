#version 330 core

out vec4 frag_color;
in vec4 out_color;
in vec2 out_uv;

uniform sampler2D text;
uniform float u_time;

void
main(void)
{
  frag_color = texture(text, out_uv).r * out_color;
}
