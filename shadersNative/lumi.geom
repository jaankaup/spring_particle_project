#version 430

layout(points) in;

uniform mat4 MVP;
uniform float radius = 0.01;
const vec4 up = vec4(0.0,radius,0.0,0.0);
const vec4 down = vec4(0.0,-radius,0.0,0.0);
const vec4 up_right = vec4(radius, radius*0.67,0.0,0.0);
const vec4 up_left = vec4(-radius, radius*0.67,0.0,0.0);
const vec4 down_right = vec4(radius, -radius*0.67,0.0,0.0);
const vec4 down_left = vec4(-radius, -radius*0.67,0.0,0.0);

layout(triangle_strip, max_vertices = 15) out;
//layout(line_strip, max_vertices = 24) out;


void createSnow() {

  vec4 pos =  gl_in[0].gl_Position;


  gl_Position = MVP * (pos + up);
  EmitVertex();

  gl_Position = MVP * (pos + down_right);
  EmitVertex();

  gl_Position = MVP * (pos + down_left);
  EmitVertex();

  gl_Position = MVP * (pos + up);
  EmitVertex();

  EndPrimitive();

  gl_Position = MVP * (pos + down);
  EmitVertex();

  gl_Position = MVP * (pos + up_left);
  EmitVertex();

  gl_Position = MVP * (pos + up_right);
  EmitVertex();

  gl_Position = MVP * (pos + down);
  EmitVertex();

  EndPrimitive();
}

void main(){
  createSnow();
}
