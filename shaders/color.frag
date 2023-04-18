#version 330 core

out vec4 frag_color;
in vec4 out_color;

void main(void)
{
  frag_color = out_color;
}
