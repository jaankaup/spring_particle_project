#version 430

layout(location = 0) in vec3 vert_in;

uniform mat4 MVP;

void main() {
  //gl_Position =  MVP * vec4(vert_in.xyz,1.0);
  gl_Position =  vec4(vert_in.xyz,1.0);
}
