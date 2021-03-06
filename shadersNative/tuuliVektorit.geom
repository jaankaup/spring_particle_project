#version 430

layout(points) in;

uniform mat4 MVP;
uniform float arrayOffset = 0.15;
uniform float arrayOffset_inner = 0.03;
uniform float windLenght_factor = 0.1;

//uniform float arrayOffset = 0.1;
//uniform float arrayOffset_inner = 0.01;
//uniform float windLenght_factor = 0.3;

layout(triangle_strip, max_vertices = 15) out;
//layout(line_strip, max_vertices = 15) out;

//out vec3 fPosIn;
out vec3 fColIn;

uniform sampler3D diffuse3DTexture;
uniform float time = 0.0;

void createArray() {

  // TODO: uniformiksi.
  
  vec4 pos =  gl_in[0].gl_Position;
  vec4 wind = vec4(texture(diffuse3DTexture,pos.xyz + vec3(time)).rgb,0.0);
  wind = wind+vec4(-0.5,-0.5,-0.5,0.0);
  vec4 wind_converted = wind * windLenght_factor;

  float dist = distance(pos.xyz,wind_converted.xyz);

//  vec4 basePos = MVP * pos;
//  vec4 endPos = MVP * (pos + wind_converted);

  vec4 basePos = pos;
  vec4 endPos = pos + wind_converted;

  vec3 up = vec3(0.0,1.0,0.0);
  vec4 right = vec4(cross(up,wind_converted.xyz),0.0);

  //vec4 rightPos = MVP * (pos + wind_converted*(1.0-arrayOffset) + arrayOffset*right);
  //vec4 leftPos = MVP * (pos + wind_converted*(1.0-arrayOffset) - arrayOffset*right);

  vec4 rightPos = pos + wind_converted*(1.0-arrayOffset) + arrayOffset*right;
  vec4 leftPos = pos + wind_converted*(1.0-arrayOffset) - arrayOffset*right;

  vec4 rightPos_inner = pos + wind_converted*(1.0-arrayOffset_inner) + arrayOffset_inner*right;
  vec4 leftPos_inner = pos + wind_converted*(1.0-arrayOffset_inner) - arrayOffset_inner*right;

  vec3 color = vec3(abs(wind.x),abs(wind.y),abs(wind.z));
  
  //////////////////////////////

  gl_Position = MVP * basePos;
  fColIn = color*2;
  EmitVertex();

  gl_Position = MVP * rightPos_inner; 
  fColIn = color*2;
  EmitVertex();

  gl_Position = MVP * leftPos_inner; 
  fColIn = color*2;
  EmitVertex();

  gl_Position = MVP *  basePos;
  fColIn = color*2;
  EmitVertex();


  EndPrimitive();

  //////////////////////////////

  gl_Position = MVP * leftPos; 
  fColIn = color*2;
  EmitVertex();

  gl_Position = MVP * endPos; 
  fColIn = color*2;
  EmitVertex();

  gl_Position = MVP * rightPos; 
  fColIn = color*2;
  EmitVertex();

  gl_Position = MVP * leftPos; 
  fColIn = color*2;
  EmitVertex();

  EndPrimitive();

}

void main(){
   createArray();
}
