#version 430

// The input data is a point. A coordinate for base cube corner. 
layout(points) in;

layout(triangle_strip, max_vertices = 15) out;

out vec3 fPosIn;
out vec3 fNormalIn;

uniform sampler3D diffuse3DTexture;
uniform sampler1D tri_table;
uniform float voxels_per_block;
uniform float isovalue = 0.0;

struct Cube
{
  vec4 v0;
  vec4 v1;
  vec4 v2;
  vec4 v3;
  vec4 v4;
  vec4 v5;
  vec4 v6;
  vec4 v7;

  vec3 n0;
  vec3 n1;
  vec3 n2;
  vec3 n3;
  vec3 n4;
  vec3 n5;
  vec3 n6;
  vec3 n7;
};


/*
        v5                        v6
         +------------------------+
        /|                       /|
       / |                      / |
      /  |                     /  |
     /   |                    /   |  
    /    |                   /    |
v1 +------------------------+ v2  |
   |     |                  |     |
   |     |                  |     |
   |     |                  |     |
   |  v4 +------------------|-----+ v7
   |    /                   |    /
   |   /                    |   /
   |  /                     |  /    
   | /                      | /
   |/                       |/
   +------------------------+
  v0                       v3


                   e5               
         +------------------------+
        /|                       /|
       / |                      / |
  e9  /  |                  e10/  |
     /   |e4                  /   |e6
    /    |       e1          /    |
   +------------------------+     |
   |     |                  |     |
   |     |                  |     |
   |     |        e7        |     |
   |     +------------------|-----+   
e0 |    /                   |e2  /
   |   /                    |   /
   |  / e8                  |  /  e11
   | /                      | /
   |/                       |/
   +------------------------+
              e3             
  +y  +z
   ^  ^
   | /
   |/
   +----> +x

 */

// https://github.com/QianMo/GPU-Gems-Book-Source-Code/blob/master/GPU-Gems-3-CD-Content/content/01/demo/models/tables.nma
// https://prideout.net/blog/distance_fields

// Noise functions copied from https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83.

//	<https://www.shadertoy.com/view/4dS3Wd>
//	By Morgan McGuire @morgan3d, http://graphicscodex.com
//
float hash(float n) { return fract(sin(n) * 1e4); }
float hash(vec2 p) { return fract(1e4 * sin(17.0 * p.x + p.y * 0.1) * (0.1 + abs(sin(p.y * 13.0 + p.x)))); }

float noise(float x) {
	float i = floor(x);
	float f = fract(x);
	float u = f * f * (3.0 - 2.0 * f);
	return mix(hash(i), hash(i + 1.0), u);
}

float noise(vec2 x) {
	vec2 i = floor(x);
	vec2 f = fract(x);

	// Four corners in 2D of a tile
	float a = hash(i);
	float b = hash(i + vec2(1.0, 0.0));
	float c = hash(i + vec2(0.0, 1.0));
	float d = hash(i + vec2(1.0, 1.0));

	// Simple 2D lerp using smoothstep envelope between the values.
	// return vec3(mix(mix(a, b, smoothstep(0.0, 1.0, f.x)),
	//			mix(c, d, smoothstep(0.0, 1.0, f.x)),
	//			smoothstep(0.0, 1.0, f.y)));

	// Same code, with the clamps in smoothstep and common subexpressions
	// optimized away.
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

// This one has non-ideal tiling properties that I'm still tuning
float noise(vec3 x) {
	const vec3 step = vec3(110, 241, 171);

	vec3 i = floor(x);
	vec3 f = fract(x);
 
	// For performance, compute the base input to a 1D hash from the integer part of the argument and the 
	// incremental change to the 1D based on the 3D -> 1D wrapping
    float n = dot(i, step);

	vec3 u = f * f * (3.0 - 2.0 * f);
	return mix(mix(mix( hash(n + dot(step, vec3(0, 0, 0))), hash(n + dot(step, vec3(1, 0, 0))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 0))), hash(n + dot(step, vec3(1, 1, 0))), u.x), u.y),
               mix(mix( hash(n + dot(step, vec3(0, 0, 1))), hash(n + dot(step, vec3(1, 0, 1))), u.x),
                   mix( hash(n + dot(step, vec3(0, 1, 1))), hash(n + dot(step, vec3(1, 1, 1))), u.x), u.y), u.z);
}

#define NUM_OCTAVES 5

float fbm(float x) {
	float v = 0.0;
	float a = 0.5;
	float shift = float(100);
	for (int i = 0; i < NUM_OCTAVES; ++i) {
		v += a * noise(x);
		x = x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}


float fbm(vec2 x) {
	float v = 0.0;
	float a = 0.5;
	vec2 shift = vec2(100);
	// Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5), -sin(0.5), cos(0.50));
	for (int i = 0; i < NUM_OCTAVES; ++i) {
		v += a * noise(x);
		x = rot * x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}


float fbm(vec3 x) {
	float v = 0.0;
	float a = 0.5;
	vec3 shift = vec3(100);
	for (int i = 0; i < NUM_OCTAVES; ++i) {
		v += a * noise(x);
		x = x * 2.0 + shift;
		a *= 0.5;
	}
	return v;
}

// MARCHING CUBES STARTS ///

float calculateDensity(vec3 v)
{
  {{density_function_comes_here_from_another_file}}
}

vec3 calculateNormal(vec3 v)
{
  float d = 1.0 / voxels_per_block;
  vec3 grad;
  grad.x = calculateDensity(v + vec3(d,0,0)) - calculateDensity(v + vec3(-d,0,0));
  grad.y = calculateDensity(v + vec3(0,d,0)) - calculateDensity(v + vec3(0,-d,0));
  grad.z = calculateDensity(v + vec3(0,0,d)) - calculateDensity(v + vec3(0,0,-d));
  return normalize(grad); 
}

Cube createCube(vec4 position)
{
  // The length of a cube edge. By increasing voxels_per_block value the cubes becomes smaller.
  float d = 1.0/voxels_per_block;

  // Create cube corner coordinates. PHASE 1. 
  vec3 p0 = position.xyz*d;
  vec3 p1 = position.xyz*d + vec3(0.0 ,   d , 0.0);
  vec3 p2 = position.xyz*d + vec3(d   ,   d , 0.0);
  vec3 p3 = position.xyz*d + vec3(d   , 0.0 , 0.0);
  vec3 p4 = position.xyz*d + vec3(0.0 , 0.0 , d);
  vec3 p5 = position.xyz*d + vec3(0.0 ,   d , d);
  vec3 p6 = position.xyz*d + vec3(d   ,   d , d);
  vec3 p7 = position.xyz*d + vec3(d   , 0.0 , d);

  // PHASE 2.
  vec4 v0 = vec4(p0, calculateDensity(p0));  
  vec4 v1 = vec4(p1, calculateDensity(p1));  
  vec4 v2 = vec4(p2, calculateDensity(p2));  
  vec4 v3 = vec4(p3, calculateDensity(p3));  
  vec4 v4 = vec4(p4, calculateDensity(p4));  
  vec4 v5 = vec4(p5, calculateDensity(p5));  
  vec4 v6 = vec4(p6, calculateDensity(p6));  
  vec4 v7 = vec4(p7, calculateDensity(p7));  

  // Create the cube. (PHASE 1 + PHASE 2)
  Cube cube;
  cube.v0 = v0;
  cube.v1 = v1;
  cube.v2 = v2;
  cube.v3 = v3;
  cube.v4 = v4;
  cube.v5 = v5;
  cube.v6 = v6;
  cube.v7 = v7;

  return cube; 
}

float calculateCase(Cube c)
{
  float result = 0.0;
  
  // PHASE 3.1 and 3.2
  if (c.v7.w < isovalue) { result += 128.0;} 
  if (c.v6.w < isovalue) { result += 64.0;}
  if (c.v5.w < isovalue) { result += 32.0;} 
  if (c.v4.w < isovalue) { result += 16.0;} 
  if (c.v3.w < isovalue) { result += 8.0; }
  if (c.v2.w < isovalue) { result += 4.0; }
  if (c.v1.w < isovalue) { result += 2.0; }
  if (c.v0.w < isovalue) { result += 1.0; }
  
  return result;
} 

vec3 interpolateV(vec4 va, vec4 vb)
{
   if (abs(isovalue - va.w) < 0.00001) { return va.xyz; }
   else if (abs(isovalue - vb.w) < 0.00001) { return vb.xyz; }
   else if (abs(va.w-vb.w) < 0.00001) { return va.xyz; }
   
   else
   {
     vec3 p;
     float mu = (isovalue - va.w) / (vb.w - va.w);
     p.x = va.x + mu * (vb.x - va.x);
     p.y = va.y + mu * (vb.y - va.y);
     p.z = va.z + mu * (vb.z - va.z);
     return p;
   }
}

vec3 interpolateN(vec3 na, vec3 nb, float densityA, float densityB)
{
   if (abs(isovalue - densityA) < 0.00001) { return na; }
   else if (abs(isovalue - densityB) < 0.00001) { return nb; }
   else if (abs(densityA-densityB) < 0.00001) { return na; }
   
   else
   {
     vec3 p;
     float mu = (isovalue - densityA) / (densityB - densityA);
     p.x = na.x + mu * (nb.x - na.x);
     p.y = na.y + mu * (nb.y - na.y);
     p.z = na.z + mu * (nb.z - na.z);
     return normalize(p);
   }
}

void createVertex(float edgeValue, Cube c)
{
    // EDGE NUMBER 0
    float iterator = 1.0 / 255.0;
    if (edgeValue == 0.0) // < 0.000001 )
    {
      gl_Position =  vec4(interpolateV(c.v0, c.v1),1.0);

        // Vertex v0 and v1 creates the edge e0. See cubes in the begining of this file.
        // Find the actual point between v0 and v1 where the isovalue is located 
        // by interpolation and create the vertex for triangle. 
        // PHASE 5.
        fPosIn = interpolateV(c.v0, c.v1);   
        
        // PHASE 6.
        // Interpolate normales in the same way and create normal vetrex for triangle.
        fNormalIn = interpolateN(c.n0, c.n1, c.v0.w, c.v1.w);

      // Create vertex.
      EmitVertex();
    }           
    // EDGE NUMBER 1
    else if (abs(edgeValue - iterator) < 0.000001)
    {

      // PHASE 5.
      gl_Position =  vec4(interpolateV(c.v1, c.v2),1.0);
      fPosIn = interpolateV(c.v1, c.v2);

      // PHASE 6.
      fNormalIn = interpolateN(c.n1, c.n2, c.v1.w, c.v2.w);
      EmitVertex();
    }           
    // EDGE NUMBER 2
    else if (abs(edgeValue - iterator * 2.0) < 0.000001)
    {
      gl_Position =  vec4(interpolateV(c.v2, c.v3),1.0);

      // PHASE 5.
      fPosIn = interpolateV(c.v2, c.v3);

      // PHASE 6.
      fNormalIn = interpolateN(c.n2, c.n3, c.v2.w, c.v3.w);
      EmitVertex();
    }           
    // EDGE NUMBER 3
    else if (abs(edgeValue - iterator * 3.0) < 0.000001)
    {
      gl_Position =  vec4(interpolateV(c.v3, c.v0),1.0);

      // PHASE 5.
      fPosIn = interpolateV(c.v3, c.v0);

      // PHASE 6.
      fNormalIn = interpolateN(c.n3, c.n0, c.v3.w, c.v0.w);
      EmitVertex();
    }           
    // EDGE NUMBER 4
    else if (abs(edgeValue - iterator * 4.0) < 0.000001)
    {
      gl_Position =  vec4(interpolateV(c.v4, c.v5),1.0);

      // PHASE 5.
      fPosIn = interpolateV(c.v4, c.v5);

      // PHASE 6.
      fNormalIn = interpolateN(c.n4, c.n5, c.v4.w, c.v5.w);
      EmitVertex();
    }           
    // EDGE NUMBER 5
    else if (abs(edgeValue - iterator * 5.0) < 0.000001)
    {
      gl_Position =  vec4(interpolateV(c.v5, c.v6),1.0);

      // PHASE 5.
      fPosIn = interpolateV(c.v5, c.v6);

      // PHASE 6.
      fNormalIn = interpolateN(c.n5, c.n6, c.v5.w, c.v6.w);
      EmitVertex();
    }           
    // EDGE NUMBER 6
    else if (abs(edgeValue - iterator * 6.0) < 0.000001)
    {
      gl_Position =  vec4(interpolateV(c.v6, c.v7),1.0);

      // PHASE 5.
      fPosIn = interpolateV(c.v6, c.v7);

      // PHASE 6.
      fNormalIn = interpolateN(c.n6, c.n7, c.v6.w, c.v7.w);
      EmitVertex();
    }           
    // EDGE NUMBER 7
    else if (abs(edgeValue - iterator * 7.0) < 0.000001)
    {
      gl_Position =  vec4(interpolateV(c.v7, c.v4),1.0);

      // PHASE 5.
      fPosIn = interpolateV(c.v7, c.v4);

      // PHASE 6.
      fNormalIn = interpolateN(c.n7, c.n4, c.v7.w, c.v4.w);
      EmitVertex();
    }           
    // EDGE NUMBER 8
    else if (abs(edgeValue - iterator * 8.0) < 0.000001)
    {
      gl_Position =  vec4(interpolateV(c.v0, c.v4),1.0);

      // PHASE 5.
      fPosIn = interpolateV(c.v0, c.v4);

      // PHASE 6.
      fNormalIn = interpolateN(c.n0, c.n4, c.v0.w, c.v4.w);
      EmitVertex();
    }           
    // EDGE NUMBER 9
    else if (abs(edgeValue - iterator * 9.0) < 0.000001)
    {
      gl_Position =  vec4(interpolateV(c.v1, c.v5),1.0);

      // PHASE 5.
      fPosIn = interpolateV(c.v1, c.v5);

      // PHASE 6.
      fNormalIn = interpolateN(c.n1, c.n5, c.v1.w, c.v5.w);
      EmitVertex();
    }           
    // EDGE NUMBER 10 
    else if (abs(edgeValue - iterator * 10.0) < 0.000001)
    {
      gl_Position =  vec4(interpolateV(c.v2, c.v6),1.0);

      // PHASE 5.
      fPosIn = interpolateV(c.v2, c.v6);

      // PHASE 6.
      fNormalIn = interpolateN(c.n2, c.n6, c.v2.w, c.v6.w);
      EmitVertex();
    }           
    // EDGE NUMBER 11 
    else if (abs(edgeValue - iterator * 11.0) < 0.000001)
    {
      gl_Position =  vec4(interpolateV(c.v3, c.v7),1.0);

      // PHASE 5
      fPosIn = interpolateV(c.v3, c.v7);

      // PHASE 6
      fNormalIn = interpolateN(c.n3, c.n7, c.v3.w, c.v7.w);
      EmitVertex();
    }           
}

void marchCube(Cube c)
{
        // PHASE 3.
        float mask = calculateCase(c); 

        // Return if the cube doesn't create any triangles.
        // The cube is totally inside or outside the surface.
        if (mask == 0.0) return;
        if (mask == 255.0) return;

        // PHASE 4.
        // Lets calculate normals for shading.
        c.n0 = calculateNormal(c.v0.xyz);
        c.n1 = calculateNormal(c.v1.xyz);
        c.n2 = calculateNormal(c.v2.xyz);
        c.n3 = calculateNormal(c.v3.xyz);
        c.n4 = calculateNormal(c.v4.xyz);
        c.n5 = calculateNormal(c.v5.xyz);
        c.n6 = calculateNormal(c.v6.xyz);
        c.n7 = calculateNormal(c.v7.xyz);

        // Find the center of the texel which has the information of the first ende vertices.
        // The pixel has the followin format: r = first edge, g = second edge, b = third edge.
        // Check the tri_table. The tri_table is now 1D texture. Index is the "pointer" to the 
        // first possible triable information. We move the pointer 0.5 forward to access the center
        // of the texel.

        //        mask + 0.5 / 1280
        //         |
        //         V
        // +----+----+----+----+----+----+----+----+----
        // |rgb |rgb |rgb |rgb |rgb |rgb |rgb |rgb |rgb|
        // +----+----+----+----+----+----+----+----+----
        //        1.   2.   3.   4.   5.
        //         

        float index = (mask * 5.0 + 0.5) / 1280.0;

        // The first edge. 
        vec3 edges1 = texture(tri_table,index).rgb;

        // Create 1. triable. PHASE 5.
        createVertex(edges1.r, c);
        createVertex(edges1.g, c);
        createVertex(edges1.b, c);

        EndPrimitive();

        // The 2. edge. PHASE 5.
        vec3 edges2 = texture(tri_table,index + 1.0/1280.0).rgb;

        // If the first vertex number is 1.0 (normalized from 255), there is no more triables.
        if (abs(edges2.r - 1.0) < 0.000001) return;

        // Create the 2. triable.
        createVertex(edges2.r, c);
        createVertex(edges2.g, c);
        createVertex(edges2.b, c);

        EndPrimitive();

        // The 3. edge. PHASE 5. 
        vec3 edges3 = texture(tri_table,index+2.0/1280.0).rgb;

        if (abs(edges3.r - 1.0) < 0.000001) return;

        createVertex(edges3.r, c);
        createVertex(edges3.g, c);
        createVertex(edges3.b, c);

        EndPrimitive();

        // The 4. edge. PHASE 5.
        vec3 edges4 = texture(tri_table,index+3.0/1280.0).rgb;

        if (abs(edges4.r - 1.0) < 0.000001) return;

        createVertex(edges4.r, c);
        createVertex(edges4.g, c);
        createVertex(edges4.b, c);

        EndPrimitive();

        // The 5. edge. PHASE 5.
        vec3 edges5 = texture(tri_table,index+4.0/1280.0).rgb;

        if (abs(edges5.r - 1.0) < 0.000001) return;
        createVertex(edges5.r, c);
        createVertex(edges5.g, c);
        createVertex(edges5.b, c);

        EndPrimitive();
}

void main(){

        // PHASE 1 and 2.
        Cube c = createCube(gl_in[0].gl_Position);

        // PHASE 3-7
        marchCube(c);
}
