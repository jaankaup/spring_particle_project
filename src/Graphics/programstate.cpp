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

void ProgramState::setParticleCount(const int particleCount)
{
   pParticleCount = particleCount;
}

int ProgramState::getParticleCount() const
{
  return pParticleCount;
}

void ProgramState::updateTick()
{
  uint32_t newTick = SDL_GetTicks();
  auto del = newTick - pTick;
  pDeltaMilliseconds = del; 
  if (del != 0)
  {
    pDeltaTime = float(newTick)/float(pTick);
  }
  pTick = newTick;
}

float ProgramState::getTimeDelta() const
{
  return pDeltaTime;
}

uint32_t ProgramState::getTimeDeltaMilliseconds() const
{
  return pDeltaMilliseconds;
}

void ProgramState::setParticlesWidth(const int width)
{
  pParticlesWidth = width;
}

void ProgramState::setParticlesHeight(const int height)
{
  pParticlesHeight = height;
}
int ProgramState::getParticlesWidth() const
{
  return pParticlesWidth;
}

int ProgramState::getParticlesHeight() const
{
  return pParticlesHeight;
}

void ProgramState::increase_h_sum(const float h)
{
  pH += h;
}

float ProgramState::get_h_sum() const
{
  return pH;
}

void ProgramState::setTimeStep(const float h)
{
  pTimeStep = h;
}

float ProgramState::getTimeStep() const
{
  return pTimeStep;
}

void ProgramState::toggleWind()
{
  pShowWind = !pShowWind;
}

bool ProgramState::getShowWind() const
{
  return pShowWind;
}

void ProgramState::toggleLumi()
{
  pLumi = !pLumi;
}

void ProgramState::toggleVerho()
{
  pVerho = !pVerho;
}

bool ProgramState::getShowLumi() const
{
  return pLumi;
}

bool ProgramState::getShowVerho() const
{
  return pVerho;
}

float ProgramState::getWindStrength() const
{
   return pWindStength;
}

void ProgramState::increaseWindStrength(const float ws)
{
  auto temp = pWindStength = pWindStength + ws;
  if (temp < 0.0) return;
  pWindStength = temp;
}

//std::string ProgramState::dimensionToString() const
//{
//  return "([" + std::to_string(pMetaData.dimensionX_min) + ","  + std::to_string(pMetaData.dimensionX_max) + "] , " +
//         "[" +  std::to_string(pMetaData.dimensionY_min)  + "," + std::to_string(pMetaData.dimensionY_max) + "] , " +
//         "[" +  std::to_string(pMetaData.dimensionZ_min)  + "," + std::to_string(pMetaData.dimensionZ_max) + "])";
//
//}
