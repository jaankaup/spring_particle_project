#version 300 es
precision highp float;

layout(location = 0) in vec3 vert_in;
layout(location = 1) in vec2 texture_in;
layout(location = 2) in vec3 normal_in;

out vec3 vFrag_in;
out vec2 tFrag_in;
out vec3 nFrag_in;

//uniform mat4 P;
//uniform mat4 V;
//uniform mat4 M;
uniform mat4 MVP;

void main(){

	//gl_Position =  P * V * M * vec4(vert_in,1);
	//gl_Position =  vec4(vert_in,1.0f);
	gl_Position =  MVP * vec4(vert_in,1);
        vFrag_in = vert_in;
        tFrag_in = texture_in;
        nFrag_in = normal_in;
}
