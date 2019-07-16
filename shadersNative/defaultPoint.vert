#version 430

layout(location = 0) in vec4 vert_in;

out vec3 vFrag_in;

uniform mat4 MVP;

void main(){

	gl_Position =  MVP * vec4(vert_in.xyz,1.0);
  //gl_PointSize = 15.0f;
  vFrag_in = vert_in.xyz;
}
