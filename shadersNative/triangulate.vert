#version 430

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_basePosition;

void main()
{
  gl_Position =  vec4(in_position + in_basePosition,1.0);
}
