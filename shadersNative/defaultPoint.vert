#version 430

layout(location = 0) in vec4 vert_in;
layout(location = 1) in vec4 velocity_in;

out vec3 vFrag_in;

uniform mat4 MVP;

void main(){

	gl_Position =  MVP * vec4(vert_in.xyz,1.0);
  vFrag_in = vert_in.xyz;
}
