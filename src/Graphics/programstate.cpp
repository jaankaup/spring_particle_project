#include "programstate.h"

ProgramState::ProgramState()
{

}

ProgramState::~ProgramState()
{

}

ProgramState& ProgramState::getInstance()
{
    static ProgramState instance;
    return instance;
}

int ProgramState::getScreenWidth() const
{
    return pScreenWidth;

}
int ProgramState::getScreenHeight() const
{
    return pScreenHeight;
}

bool ProgramState::getAppRunning() const
{
    return pAppRunning;
}

bool ProgramState::getWireframe() const
{
  return pWireframe;
}
 
void ProgramState::setScreenWidth(const int width)
{
  if (width > 0) pScreenWidth = width;

}

void ProgramState::setScreenHeight(const int height)
{
  if (height > 0) pScreenHeight = height;
}

void ProgramState::setAppRunning(const bool running)
{
  pAppRunning = running;
}

void ProgramState::setVoxelsPerBlock(const float count)
{
  if (count < 1.0)
  {

    Log::getError().log("ProgramState::setVoxelsPerBlock(%)",std::to_string(count));
    Log::getError().log("ProgramState::setVoxelsPerBlock: count must be >= 1.0. Omitting changes.",std::to_string(count));
    return;
  }
  pVoxels_per_count = count;
}

void ProgramState::setWireframe(const bool enableWireframe)
{
  pWireframe = enableWireframe;
}

float ProgramState::getVoxelsPerBlock() const
{
  return pVoxels_per_count;
}

void ProgramState::setStartPoint(const glm::vec3& point)
{
  pStartPoint = point;
}

glm::vec3 ProgramState::getStartPoint() const
{
  return pStartPoint;
}

float ProgramState::getCubeMask() const
{
  return pCubeMask;
}

void ProgramState::setCubeMask(const float cubeMask)
{
  if (cubeMask < 256.0f && cubeMask > -1.0f) pCubeMask = cubeMask; 
  else return;
  
//  Log::getError().log("ProgramState::setCubeMask(%)",std::to_string(cubeMask));
//  Log::getError().log("Illegal argument '%'", std::to_string(cubeMask));
}

float ProgramState::getCubeMaskCeil() const
{
  return std::floor(pCubeMask);
}

void ProgramState::setDebugCube(const bool debugCube)
{
  pDebugCube = debugCube;
}

bool ProgramState::getDebugCube() const
{
  return pDebugCube;
}

void ProgramState::setCubeDimension(const glm::ivec3& cubeDimension)
{
  pCubeDimension = cubeDimension;
}

glm::ivec3 ProgramState::getCubeDimension() const
{
  return pCubeDimension;
}

Cube_meta_data* ProgramState::getMetadata()
{
  return &pMetaData;
}

//std::string ProgramState::dimensionToString() const
//{
//  return "([" + std::to_string(pMetaData.dimensionX_min) + ","  + std::to_string(pMetaData.dimensionX_max) + "] , " +
//         "[" +  std::to_string(pMetaData.dimensionY_min)  + "," + std::to_string(pMetaData.dimensionY_max) + "] , " +
//         "[" +  std::to_string(pMetaData.dimensionZ_min)  + "," + std::to_string(pMetaData.dimensionZ_max) + "])";
//
//}
