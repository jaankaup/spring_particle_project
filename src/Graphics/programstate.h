#ifndef PROGRAMSATE_H
#define PROGRAMSATE_H
//#include <iostream>
//#include <vector>
//#include <array>
//#include <GL/glew.h>
#include <cmath>
#include <tuple>
#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>
#include "vertexbuffer.h"
#include "../Utils/log.h"

  /* This is the information for the marching cubes.
   *
   * @block_size :: how many times the cube (unit cube) is divided. For example
   *               if block_size is 4, then the unit cube is split to 16 cubes.
   * @cube_count_x :: how many unit cubes must be created in x dimension.
   * @cube_count_y :: how many unit cubes must be created in y dimension
   * @cube_count_z :: how many unit cubes must be created in z dimension.
   *
   * For example if block_size if 4 and all cube_count_n are 8, then 
   * we have 8x8x8 of unit cubes in the world, and each cube has 16 inner
   * cubes. So total number of cubes to march is 8x8x8x4.
   *
   * @meshShader :: the name of the shader that must be binded when drawing the 
   *               triangulated data. 
   *
   * @triangulationshader :: the name of the shader that runs the cube march
   * and the output will be stored as triangle mest
   *
   * @cubeMarchShader :: the name of the shader that runs the cube march on 
   *                     each frame. For debuggin purpose only.
   *
   * @cubeMarchWireframe :: the name of the shader that draw the wireframe of 
   *                        the @cubeMarchShader. For debuggin purpose only.
   *
   * @base_vertex_buffer_name :: The name of the vertexbuffer object that holds
   * all the bottom left points of each cube. 
   *
   * @isovalue :: This the density value that might be in the surface. If at
   * some point in the cube some corner density value is greater than @isovalue 
   * and some other point has smaller value than @isovalue we know that the
   * cube is on the surface and it will create 1-5 triangles based on the cube
   * case. If all the values of cube corners are > @isovalue or < @isovalue,
   * then the cube is totally outside of the surface or totally inside. Then 
   * it won't produce any triangles.
   * */
  struct Cube_meta_data {
    int block_size = 16;
    int cube_count_x = 0;
    int cube_count_y = 0;
    int cube_count_z = 0;
    std::string meshShader;
    std::string triangulationShader;
    std::string cubeMarchShader;
    std::string cubeMarchWireframe;
    std::string base_vertex_buffer_name;
    std::string texture3Dname;
    std::string tri_table_name;
    float isovalue = 0.0;
//    int dimensionX_min = -8; 
//    int dimensionY_min = -8;  
//    int dimensionZ_min = -8;
//    int dimensionX_max = 8;
//    int dimensionY_max = 8;  
//    int dimensionZ_max = 8;
    int activeShaderNumber = 1;
    // The xMin,yMin,zMin,xMax,yMax,zMax,voxels_per_block
    std::vector<std::tuple<int,int,int,int,int,int,int>> dimensionsPerDF;
    };

class ProgramState
{ 

	public:

    static ProgramState& getInstance();
		~ProgramState();

    int getScreenWidth() const;
    int getScreenHeight() const;
    bool getAppRunning() const;
    float getVoxelsPerBlock() const;
    bool getWireframe() const;
    glm::vec3 getStartPoint() const;
    float getCubeMask() const;
    float getCubeMaskCeil() const;
    bool getDebugCube() const;
    glm::ivec3 getCubeDimension() const;
    Cube_meta_data* getMetadata();


    void setScreenWidth(const int width);
    void setScreenHeight(const int height);
    void setAppRunning(const bool running);
    void setVoxelsPerBlock(const float count);
    void setWireframe(const bool wireframeEnabled);
    void setStartPoint(const glm::vec3& point);
    void setCubeMask(const float cubeMask);
    void setDebugCube(const bool debugCube);
    void setCubeDimension(const glm::ivec3& cubeDimension);
//    std::string dimensionToString() const;

	private:

		ProgramState();
    int pScreenWidth = 1200;
    int pScreenHeight = 800;
    bool pAppRunning = true;
    float pVoxels_per_count = 1.0;
    bool pWireframe = false;
    glm::vec3 pStartPoint = glm::vec3(0.0f,0.0f,0.0f);
    float pCubeMask = 0.1f;
    bool pDebugCube = false;
    glm::ivec3 pCubeDimension;
    Cube_meta_data pMetaData;

};


#endif // PROGRAMSATE_H
