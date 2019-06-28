#include "model.h"

Model::Model()
{
}

Model::~Model()
{
}

void Model::addCommand(const Command& command)
{
  pCommands.push_back(command);
}

void Model::setCameraPosition(const glm::vec3& cPos)
{
    pCameraStart = cPos;
}

glm::vec3 Model::getCameraPosition() const
{
  return pCameraStart;  
}

void Model::setDisabled(const bool disabled)
{
  pDisabled = disabled;
}

bool Model::getDisabled() const
{
  return pDisabled;
}

//void Model::addModelMatrix(const glm::mat4& modelMatrix)
//{
//  pModelMatrix = modelMatrix;
//}

//glm::mat4 Model::getModelMatrix() const
//{
//  return pModelMatrix;
//}

std::vector<Command>* Model::getCommands() const
{
  return const_cast<std::vector<Command>*>(&pCommands);
}
