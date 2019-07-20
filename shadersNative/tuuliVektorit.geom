#version 430

layout(points) in;

uniform mat4 MVP;

//layout(triangle_strip, max_vertices = 3) out;
layout(line_strip, max_vertices = 15) out;

//out vec3 fPosIn;
out vec3 fColIn;

uniform sampler3D diffuse3DTexture;
uniform float time = 0.0;

void createArray() {

  float arrayOffset = 0.1;
  float windLenght_factor = 0.3;
  
  vec4 pos =  gl_in[0].gl_Position;
  vec4 wind = vec4(texture(diffuse3DTexture,pos.xyz + vec3(time)).rgb,0.0);
  wind = wind+vec4(-0.5,-0.5,-0.5,0.0);
  wind *= windLenght_factor;

  vec4 basePos = MVP * pos;
  vec4 endPos = MVP * (pos + wind);

  vec3 up = vec3(0.0,1.0,0.0);
  vec4 right = vec4(cross(up,wind.xyz),0.0);

  vec4 rightPos = MVP * (pos + wind*(1.0-arrayOffset) + arrayOffset*right);
  vec4 leftPos = MVP * (pos + wind*(1.0-arrayOffset) - arrayOffset*right);
  //vec4 left = MVP * (pos + wind*0.2 + vec4(-0.1,0.0,0.0,0.0));

  vec3 color = wind.xyz*2;
  
  //////////////////////////////

  gl_Position = basePos;
  fColIn = color;
  EmitVertex();

  gl_Position = endPos; 
  fColIn = color;
  EmitVertex();

  EndPrimitive();

  //////////////////////////////

  gl_Position = endPos;
  fColIn = color;
  EmitVertex();

  gl_Position = rightPos; 
  fColIn = wind.xyz*2;
  EmitVertex();

  EndPrimitive();

  //////////////////////////////

  gl_Position = endPos;
  fColIn = color;
  EmitVertex();

  gl_Position = leftPos; 
  fColIn = wind.xyz*2;
  EmitVertex();

  EndPrimitive();

  //////////////////////////////



}

void main(){
   createArray();
}
