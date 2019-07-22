#version 430

layout(points) in;

uniform mat4 MVP;

in float geom_in[];
//in float geom_in;

struct PVdata
{
  vec4 pos;
  vec4 vel;
};

struct StaticData
{
  vec4 some_data;
  vec4 friends;
  vec4 friends2;
  vec4 friends3;
  vec4 rest_forces;
  vec4 rest_forces2;
  vec4 rest_forces3;
};

layout (std430, binding = 0) buffer InitialBuf { PVdata initial[]; };
layout (std430, binding = 1) buffer StaticBuf { StaticData static_data[]; };

//layout(triangle_strip, max_vertices = 3) out;
layout(line_strip, max_vertices = 24) out;

//out vec3 fColIn;

void createLine(vec4 a, vec4 b) {

  gl_Position = MVP * a;
  EmitVertex();

  gl_Position = MVP * b; 
  EmitVertex();

  EndPrimitive();

}

void main(){

   PVdata base = initial[int(geom_in[0])];
   StaticData base_static = static_data[int(geom_in[0])];

   float friend0 = base_static.friends.x;
   float friend1 = base_static.friends.y;
   float friend2 = base_static.friends.z;
   float friend3 = base_static.friends.w;
   float friend4 = base_static.friends2.x;
   float friend5 = base_static.friends2.y;
   float friend6 = base_static.friends2.z;
   float friend7 = base_static.friends2.w;
   float friend8 = base_static.friends3.x;
   float friend9 = base_static.friends3.y;
   float friend10 = base_static.friends3.z;
   float friend11 = base_static.friends3.w;

   PVdata temp;

   if (friend0 != -1.0)
   {
     temp = initial[int(friend0)]; 
     createLine(base.pos,temp.pos);
   }

   if (friend1 != -1.0)
   {
     temp = initial[int(friend1)]; 
     createLine(base.pos,temp.pos);
   }

   if (friend2 != -1.0)
   {
     temp = initial[int(friend2)]; 
     createLine(base.pos,temp.pos);
   }

   if (friend3 != -1.0)
   {
     temp = initial[int(friend3)]; 
     createLine(base.pos,temp.pos);
   }

   if (friend4 != -1.0)
   {
     temp = initial[int(friend4)]; 
     createLine(base.pos,temp.pos);
   }

   if (friend5 != -1.0)
   {
     temp = initial[int(friend5)]; 
     createLine(base.pos,temp.pos);
   }

   if (friend6 != -1.0)
   {
     temp = initial[int(friend6)]; 
     createLine(base.pos,temp.pos);
   }

   if (friend7 != -1.0)
   {
     temp = initial[int(friend7)]; 
     createLine(base.pos,temp.pos);
   }

   if (friend8 != -1.0)
   {
     temp = initial[int(friend8)]; 
     createLine(base.pos,temp.pos);
   }

   if (friend9 != -1.0)
   {
     temp = initial[int(friend9)]; 
     createLine(base.pos,temp.pos);
   }

   if (friend10 != -1.0)
   {
     temp = initial[int(friend10)]; 
     createLine(base.pos,temp.pos);
   }

   if (friend11 != -1.0)
   {
     temp = initial[int(friend11)]; 
     createLine(base.pos,temp.pos);
   }
}
