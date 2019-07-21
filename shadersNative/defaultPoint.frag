#version 430
in vec3 vFrag_in;

uniform vec3 input_color;

out vec3 color;

void main(){
  color = input_color;
}
