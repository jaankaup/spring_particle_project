#version 430

//in vec3 vFrag_in;
in vec3 fColIn;

out vec4 color;

void main(){
  color = vec4(fColIn,1.0);
}
