#version 430

layout(location = 0) in vec4 vert_in;
layout(location = 1) in vec4 velocity_in;

void main() {
  gl_Position = vert_in;
}
