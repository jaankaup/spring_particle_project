#ifndef MARCHINGCUBES_H
#define MARCHINGCUBES_H
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "voxeldata.h"
#include "../Utils/log.h"

//extern std::vector<glm::vec3> createSomething();


// For each cell in data set.
// 1. Examine a cell
// 2. Classify each vertex as inside or outside
// 3. Build an index
// 4. Get edge list from table[index]
// 5. Interpolate the edge location
// 6. Compute gradients
// 7. Consider ambiguous cases
// 8. Go to next cell

/*
        v4         e4             v5
         +------------------------+
        /|                       /|
       / |                      / |
  e7  /  |                  e5 /  |
     /   |e8                  /   |e9
    /    |       e6          /    |
v7 +------------------------+ v6  |
   |     |                  |     |
   |     |                  |     |
   |     |        e0        |     |
   |  v0 +------------------|-----+ v1
e11|    /                   |e10 /
   |   /                    |   /
   |  / e3                  |  /  e1
   | /                      | /
   |/                       |/
   +------------------------+
  v3          e2           v2

 v(n) vertex-index.
 e(n) edge-index.

 */

//marching cubes table data
static const std::array<int,256> lookupTable =  {
0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };

//char triTable[256][16] =
static const std::array<std::array<char,16>,256> triTable =  {{
  {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1}},
  {{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1}},
  {{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1}},
  {{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1}},
  {{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1}},
  {{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1}},
  {{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1}},
  {{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1}},
  {{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1}},
  {{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1}},
  {{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1}},
  {{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1}},
  {{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1}},
  {{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1}},
  {{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1}},
  {{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1}},
  {{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1}},
  {{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1}},
  {{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1}},
  {{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1}},
  {{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1}},
  {{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1}},
  {{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1}},
  {{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1}},
  {{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1}},
  {{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1}},
  {{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1}},
  {{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1}},
  {{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1}},
  {{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1}},
  {{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1}},
  {{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1}},
  {{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1}},
  {{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1}},
  {{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1}},
  {{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1}},
  {{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1}},
  {{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1}},
  {{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1}},
  {{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1}},
  {{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1}},
  {{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1}},
  {{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1}},
  {{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1}},
  {{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1}},
  {{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1}},
  {{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1}},
  {{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1}},
  {{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1}},
  {{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1}},
  {{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1}},
  {{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1}},
  {{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1}},
  {{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1}},
  {{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1}},
  {{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1}},
  {{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1}},
  {{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1}},
  {{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1}},
  {{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1}},
  {{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1}},
  {{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1}},
  {{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1}},
  {{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1}},
  {{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1}},
  {{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1}},
  {{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1}},
  {{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1}},
  {{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1}},
  {{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1}},
  {{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1}},
  {{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1}},
  {{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1}},
  {{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1}},
  {{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1}},
  {{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1}},
  {{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1}},
  {{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1}},
  {{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1}},
  {{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1}},
  {{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1}},
  {{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1}},
  {{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1}},
  {{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1}},
  {{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1}},
  {{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1}},
  {{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1}},
  {{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1}},
  {{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1}},
  {{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1}},
  {{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1}},
  {{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1}},
  {{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1}},
  {{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1}},
  {{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1}},
  {{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1}},
  {{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1}},
  {{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1}},
  {{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1}},
  {{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1}},
  {{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1}},
  {{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1}},
  {{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1}},
  {{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1}},
  {{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1}},
  {{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1}},
  {{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1}},
  {{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1}},
  {{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1}},
  {{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1}},
  {{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1}},
  {{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1}},
  {{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1}},
  {{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1}},
  {{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1}},
  {{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1}},
  {{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1}},
  {{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1}},
  {{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1}},
  {{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1}},
  {{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1}},
  {{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1}},
  {{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1}},
  {{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1}},
  {{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1}},
  {{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1}},
  {{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1}},
  {{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1}},
  {{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1}},
  {{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1}},
  {{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1}},
  {{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1}},
  {{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1}},
  {{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1}},
  {{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1}},
  {{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1}},
  {{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1}},
  {{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1}},
  {{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1}},
  {{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1}},
  {{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1}},
  {{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}},
  {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}}
}};

extern glm::vec3 interPolateV(float isolevel,const glm::vec3& p1, const glm::vec3& p2,float valp1, float valp2);
extern glm::vec3 interPolateN(float isolevel,const glm::vec3& n1, const glm::vec3& n2);

template<typename T>
glm::vec3 calculateNormal(const int i, const int j, const int k, const VoxelData<T>& data)
{
    float gx = data.getValue(i+1  ,j  ,k) - data.getValue(i-1, j, k);
    float gy = data.getValue(i  ,j+1  ,k) - data.getValue(i, j-1, k);
    float gz = data.getValue(i  ,j  ,k+1) - data.getValue(i, j, k-1);

    glm::vec3 v(gx,gy,gz);
    if (gx == 0.0f && gy == 0.0f && gz == 0.0f) return glm::vec3(0.0f);
//    Log::getDebug().log("calcNormal = (%,%,%)", std::to_string(v.x),std::to_string(v.y),std::to_string(v.z));
//    glm::vec3 n = glm::normalize(v);
    return glm::normalize(v);
//    Log::getDebug().log("normalizedcalcNormal = (%,%,%)", std::to_string(n.x),std::to_string(n.y),std::to_string(n.z));
//    return glm::normalize(v);
//    return glm::vec3(gx,gy,gz);
}

/* @param data is the set of cells to travel. @param isovalue is the to
 * determine if a corner of a cell is inside or outside of the surface.
 */
template <typename ArrayType>
std::tuple<std::vector<glm::vec3>,std::vector<glm::vec3>> triangulate(const ArrayType& data, float isolevel)
{
  std::vector<glm::vec3> vertices; 
  std::vector<glm::vec3> result_normals; 

  std::array<glm::vec3,12> edgeVertices;
  std::array<glm::vec3,12> edgeNormals;
  //glm::ivec3 positionIndex = data[0].pPosition;

  // Phase 2. Check whetever the cell corners are inside the surface (1) or outside the
  // surface (0).
  // 3. Build an index

  auto [xDim,yDim,zDim] = data.getDimensions();
//  int indexCount = (xDim-1)*(yDim-1)*(zDim-1);
//  Log::getDebug().log("INDEXCOUNT = %",std::to_string(indexCount));
//  int laskuri = 0;
  for (int k=0 ; k<zDim-1; k++)
  {
  for (int j=0 ; j<yDim-1; j++)
  {
  for (int i=0 ; i<xDim-1; i++)
  {
 //   Log::getDebug().log("IND (%,%,%)",std::to_string(i),std::to_string(j),std::to_string(k));
    int cubeindex = 0;

    // Construct the cube.
    std::vector<float> isovalues(8);
    std::vector<glm::vec3> coordinates(8);
    std::vector<glm::vec3> normals(8);

//    Log::getDebug().log("laskuri = %",std::to_string(laskuri));
//    laskuri++;

    isovalues[0] = data.getValue(i  ,j  ,k);
    isovalues[1] = data.getValue(i+1,j  ,k);
    isovalues[2] = data.getValue(i+1,j  ,k+1);
    isovalues[3] = data.getValue(i  ,j  ,k+1);
    isovalues[4] = data.getValue(i  ,j+1,k);
    isovalues[5] = data.getValue(i+1,j+1,k);
    isovalues[6] = data.getValue(i+1,j+1,k+1);
    isovalues[7] = data.getValue(i  ,j+1,+k+1);


    coordinates[0] = glm::vec3((float)i,(float)j,float(k));
    coordinates[1] = glm::vec3((float)(i+1),(float)j,(float)k);
    coordinates[2] = glm::vec3((float)(i+1),(float)j,(float)(k+1));
    coordinates[3] = glm::vec3((float)i,(float)j,(float)(k+1));
    coordinates[4] = glm::vec3((float)i,(float)(j+1),(float)k);
    coordinates[5] = glm::vec3((float)(i+1),(float)(j+1),(float)k);
    coordinates[6] = glm::vec3((float)(i+1),(float)(j+1),(float)(k+1));
    coordinates[7] = glm::vec3((float)i,(float)(j+1),(float)(k+1));
//
//    isovalues[0] = data.getValue(i  ,j  ,k);
//    isovalues[1] = data.getValue(i+1,j  ,k);
//    isovalues[5] = data.getValue(i+1,j  ,k+1);
//    isovalues[4] = data.getValue(i  ,j  ,k+1);
//    isovalues[3] = data.getValue(i  ,j+1,k);
//    isovalues[2] = data.getValue(i+1,j+1,k);
//    isovalues[6] = data.getValue(i+1,j+1,k+1);
//    isovalues[7] = data.getValue(i  ,j+1,+k+1);
//
//
//    coordinates[0] = glm::vec3((float)i,(float)j,float(k));
//    coordinates[1] = glm::vec3((float)(i+1),(float)j,(float)k);
//    coordinates[5] = glm::vec3((float)(i+1),(float)j,(float)(k+1));
//    coordinates[4] = glm::vec3((float)i,(float)j,(float)(k+1));
//    coordinates[3] = glm::vec3((float)i,(float)(j+1),(float)k);
//    coordinates[2] = glm::vec3((float)(i+1),(float)(j+1),(float)k);
//    coordinates[6] = glm::vec3((float)(i+1),(float)(j+1),(float)(k+1));
//    coordinates[7] = glm::vec3((float)i,(float)(j+1),(float)(k+1));
//
    normals[0] = calculateNormal(i  ,j  ,k, data);
    normals[1] = calculateNormal(i+1,j  ,k, data);
    normals[2] = calculateNormal(i+1,j  ,k+1, data);
    normals[3] = calculateNormal(i  ,j  ,k+1, data);
    normals[4] = calculateNormal(i  ,j+1,k, data);
    normals[5] = calculateNormal(i+1,j+1,k, data);
    normals[6] = calculateNormal(i+1,j+1,k+1, data);
    normals[7] = calculateNormal(i  ,j+1,+k+1, data);
    std::string cubeindexBinary = "00000000";

    if (isovalues[0] < isolevel) { cubeindex |= 1;   cubeindexBinary[0] = '1';}
    if (isovalues[1] < isolevel) { cubeindex |= 2;   cubeindexBinary[1] = '1';}  
    if (isovalues[2] < isolevel) { cubeindex |= 4;   cubeindexBinary[2] = '1';} 
    if (isovalues[3] < isolevel) { cubeindex |= 8;   cubeindexBinary[3] = '1';}
    if (isovalues[4] < isolevel) { cubeindex |= 16;  cubeindexBinary[4] = '1';}
    if (isovalues[5] < isolevel) { cubeindex |= 32;  cubeindexBinary[5] = '1';}
    if (isovalues[6] < isolevel) { cubeindex |= 64;  cubeindexBinary[6] = '1';}
    if (isovalues[7] < isolevel) { cubeindex |= 128; cubeindexBinary[7] = '1';}
  
    Log::getDebug().log("CUBEINDEX = %",std::to_string(cubeindex));
    Log::getDebug().log("CUBEINDEX = %",cubeindexBinary);
//    Log::getDebug().log("0 : % < %",std::to_string(isovalues[0]),std::to_string(isolevel));
//    Log::getDebug().log("1 : % < %",std::to_string(isovalues[1]),std::to_string(isolevel));
//    Log::getDebug().log("2 : % < %",std::to_string(isovalues[2]),std::to_string(isolevel));
//    Log::getDebug().log("3 : % < %",std::to_string(isovalues[3]),std::to_string(isolevel));
//    Log::getDebug().log("4 : % < %",std::to_string(isovalues[4]),std::to_string(isolevel));
//    Log::getDebug().log("5 : % < %",std::to_string(isovalues[5]),std::to_string(isolevel));
//    Log::getDebug().log("6 : % < %",std::to_string(isovalues[6]),std::to_string(isolevel));
//    Log::getDebug().log("7 : % < %",std::to_string(isovalues[7]),std::to_string(isolevel));
//    Log::getDebug().log("cubeindex = %", std::to_string(cubeindex));
//  for (const auto& a : coordinates)
//  {
//    Log::getDebug().log("POSITIONS (%,%,%)", std::to_string(a.x), std::to_string(a.y),std::to_string(a.z));
//  }
//  for (int b=0 ; b<isovalues.size(); b++)
//  {
//    Log::getDebug().log("VALUES %", std::to_string(isovalues[b]));
//  }
 
  // 4. Get edge list from table[index]
//  std::array<char,16> edgeIncides = triTable[lookupTable_index]; 

  // 5. Interpolate the edge location
  /* Cube is entirely in/out of the surface */

  //std::vector<glm::vec3> vertices;

   if (lookupTable[cubeindex] == 0) continue;
   
//   std::array<float, 8> grad;
//   float maxX = 3.0;
//   float maxY = 3.0;
//   float maxZ = 3.0;

   else {
   /* Find the vertices where the surface intersects the cube */
     if (lookupTable[cubeindex] & 1) {
//        Log::getDebug().log("lookupTable[%] & 1.",std::to_string(cubeindex));
        edgeVertices[0] = interPolateV(isolevel,coordinates[0],coordinates[1],isovalues[0],isovalues[1]);
        edgeNormals[0] = glm::normalize(interPolateV(isolevel,normals[0],normals[1],isovalues[0],isovalues[1]));
        //edgeNormals[0] = glm::normalize(interPolateN(isolevel,normals[0],normals[1]));
     }
     if (lookupTable[cubeindex] & 2) {
        //vertlist[1] =
//        Log::getDebug().log("lookupTable[%] & 2.",std::to_string(cubeindex));
           edgeVertices[1] = interPolateV(isolevel,coordinates[1],coordinates[2],isovalues[1],isovalues[2]);
           edgeNormals[1] = glm::normalize(interPolateV(isolevel,normals[1],normals[2],isovalues[1],isovalues[2]));
//           edgeNormals[1] = glm::normalize(interPolateN(isolevel,normals[1],normals[2]));
     }
     if (lookupTable[cubeindex] & 4) {
        //vertlist[2] =
//        Log::getDebug().log("lookupTable[%] & 4.",std::to_string(cubeindex));
           edgeVertices[2] = interPolateV(isolevel,coordinates[2],coordinates[3],isovalues[2],isovalues[3]);
           edgeNormals[2] = glm::normalize(interPolateV(isolevel,normals[2],normals[3],isovalues[2],isovalues[3]));
//           edgeNormals[2] = glm::normalize(interPolateN(isolevel,normals[2],normals[3]));
     }
     if (lookupTable[cubeindex] & 8)
     {
 //       Log::getDebug().log("lookupTable[%] & 8.",std::to_string(cubeindex));
        //vertlist[3] =
           edgeVertices[3] = interPolateV(isolevel,coordinates[3],coordinates[0],isovalues[3],isovalues[0]);
           edgeNormals[3] = glm::normalize(interPolateV(isolevel,normals[3],normals[0],isovalues[3],isovalues[0]));
           //edgeNormals[3] = glm::normalize(interPolateN(isolevel,normals[3],normals[0]));
     }
     if (lookupTable[cubeindex] & 16)
     {
//        Log::getDebug().log("lookupTable[%] & 16.",std::to_string(cubeindex));
        //vertlist[4] =
           edgeVertices[4] = interPolateV(isolevel,coordinates[4],coordinates[5],isovalues[4],isovalues[5]);
           edgeNormals[4] = glm::normalize(interPolateV(isolevel,normals[4],normals[5],isovalues[4],isovalues[5]));
//           edgeNormals[4] = glm::normalize(interPolateN(isolevel,normals[4],normals[5]));
     }
     if (lookupTable[cubeindex] & 32)
     {
//        Log::getDebug().log("lookupTable[%] & 32.",std::to_string(cubeindex));
        //vertlist[5] =
           edgeVertices[5] = interPolateV(isolevel,coordinates[5],coordinates[6],isovalues[5],isovalues[6]);
           edgeNormals[5] = glm::normalize(interPolateV(isolevel,normals[5],normals[6],isovalues[5],isovalues[6]));
//           edgeNormals[5] = glm::normalize(interPolateN(isolevel,normals[5],normals[6]));
     }
     if (lookupTable[cubeindex] & 64)
     {
//        Log::getDebug().log("lookupTable[%] & 64.",std::to_string(cubeindex));
        //vertlist[6] =
           edgeVertices[6] = interPolateV(isolevel,coordinates[6],coordinates[7],isovalues[6],isovalues[7]);
           edgeNormals[6] = glm::normalize(interPolateV(isolevel,normals[6],normals[7],isovalues[6],isovalues[7]));
//           edgeNormals[6] = glm::normalize(interPolateN(isolevel,normals[6],normals[7]));
     }
     if (lookupTable[cubeindex] & 128)
     {
//        Log::getDebug().log("lookupTable[%] & 128.",std::to_string(cubeindex));
        //vertlist[7] =
           edgeVertices[7] = interPolateV(isolevel,coordinates[7],coordinates[4],isovalues[7],isovalues[4]);
           edgeNormals[7] = glm::normalize(interPolateV(isolevel,normals[7],normals[4],isovalues[7],isovalues[4]));
//           edgeNormals[7] = glm::normalize(interPolateN(isolevel,normals[7],normals[4]));
     }
     if (lookupTable[cubeindex] & 256)
     {
//        Log::getDebug().log("lookupTable[%] & 256.",std::to_string(cubeindex));
        //vertlist[8] =
           edgeVertices[8] = interPolateV(isolevel,coordinates[0],coordinates[4],isovalues[0],isovalues[4]);
           edgeNormals[8] = glm::normalize(interPolateV(isolevel,normals[0],normals[4],isovalues[0],isovalues[4]));
//           edgeNormals[8] = glm::normalize(interPolateN(isolevel,normals[0],normals[4]));
     }
     if (lookupTable[cubeindex] & 512)
     {
        //vertlist[9] =
           edgeVertices[9] = interPolateV(isolevel,coordinates[1],coordinates[5],isovalues[1],isovalues[5]);
           edgeNormals[9] = glm::normalize(interPolateV(isolevel,normals[1],normals[5],isovalues[1],isovalues[5]));
//           edgeNormals[9] = glm::normalize(interPolateN(isolevel,normals[1],normals[5]));
     }
     if (lookupTable[cubeindex] & 1024)
     {
        //vertlist[10] =
           edgeVertices[10] = interPolateV(isolevel,coordinates[2],coordinates[6],isovalues[2],isovalues[6]);
           edgeNormals[10] = glm::normalize(interPolateV(isolevel,normals[2],normals[6],isovalues[2],isovalues[6]));
//           edgeNormals[10] = glm::normalize(interPolateN(isolevel,normals[2],normals[6]));
     }
     if (lookupTable[cubeindex] & 2048)
     {
        //vertlist[11] =
           edgeVertices[11] = interPolateV(isolevel,coordinates[3],coordinates[7],isovalues[3],isovalues[7]);  
           edgeNormals[11] = glm::normalize(interPolateV(isolevel,normals[3],normals[7],isovalues[3],isovalues[7]));
//           edgeNormals[11] = glm::normalize(interPolateN(isolevel,normals[3],normals[7]));
     }
   }
      //int ntriang = 0;
      for (int g=0;triTable[cubeindex][g]!=-1;g+=3) {
      
//      Log::getDebug().log("TRIANGLE FOUND");
      vertices.push_back(edgeVertices[triTable[cubeindex][g]]);
      vertices.push_back(edgeVertices[triTable[cubeindex][g+1]]);
      vertices.push_back(edgeVertices[triTable[cubeindex][g+2]]);
      result_normals.push_back(edgeNormals[triTable[cubeindex][g]]);
      result_normals.push_back(edgeNormals[triTable[cubeindex][g+1]]);
      result_normals.push_back(edgeNormals[triTable[cubeindex][g+2]]);
//      Log::getDebug().log("VERTICES SIZE %", std::to_string(vertices.size()));
//      Log::getDebug().log("NORMALS SIZE %", std::to_string(result_normals.size()));
//      triangles[ntriang].p[0] = vertlist[edgeVertices[cubeindex][i  ]];
//      triangles[ntriang].p[1] = vertlist[edgeVertices[cubeindex][i+1]];
//      triangles[ntriang].p[2] = vertlist[edgeVertices[cubeindex][i+2]];
      //ntriang++;
   }
  }}} // fors.

//  std::vector<glm::vec3> result;
  return std::make_tuple(vertices,result_normals);
}

//extern template<typename ArrayType>
//std::vector<glm::vec3> triangulate(const ArrayType& data, float isolevel);

  
#endif // MARCHINGCUBES_H
