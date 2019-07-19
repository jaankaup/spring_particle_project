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
  vec4 pos =  gl_in[0].gl_Position;
  vec4 wind = vec4(texture(diffuse3DTexture,pos.xyz + vec3(time)).rgb,0.0);

  gl_Position = (MVP * pos);
  //fPosIn = (MVP * pos).xyz;
  fColIn = wind.xyz;
  EmitVertex();

  //fPosIn = pos + vec3(1.0,1.0,1.0); // wind;
  gl_Position = MVP * vec4(pos + wind*0.3); // wind;
  //fPosIn = (MVP * (pos + wind*0.3)).xyz;
  fColIn = wind.xyz;
  EmitVertex();

  EndPrimitive();






//  gl_Position = MVP * (vec4(pos,1.0) + vec4(0.1,0.1,0.1,1.0)); // wind;
//  EmitVertex();
//  fPosIn = pos + vec3(0.01,0.01,0.01);
//  EmitVertex();

}

void main(){
   createArray();
}
