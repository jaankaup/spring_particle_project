#version 430
//precision highp float;
//precision mediump float;

layout(location = 0) in vec3 vert_in;
layout(location = 1) in vec3 normal_in;

out vec3 vFrag_in;
out vec3 nFrag_in;

uniform mat4 MVP;

void main(){

	gl_Position =  MVP * vec4(vert_in,1);
  vFrag_in = vert_in;
  nFrag_in = normal_in;
}
