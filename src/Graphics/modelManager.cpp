#include "modelManager.h"

ModelManager::ModelManager()
{

}

ModelManager::~ModelManager()
{
//   for (const auto& data : pModels)
//   {
//     std::get<1>(data).dispose();
//   }
}

Model* ModelManager::createModel(const std::string& name)
{
  Model s;
  auto modelData = std::make_tuple(name,s);
  pModels.push_back(modelData);
  return getModelByName(name);
}

ModelManager& ModelManager::getInstance()
{
    static ModelManager instance;
    return instance;
}

Model* ModelManager::getModelByName(const std::string& name) const
{
    for (const auto& obj : pModels)
    {
        if (std::get<0>(obj) == name)
        {
            return const_cast<Model*>(&std::get<1>(obj));
        }
    }
    throw std::runtime_error("ModelManager::getModelByName(" + name + "). No such model.");
}

Model* ModelManager::createSceneObject()
{

  deleteModel("sceneModel");
  glm::mat4 original = glm::mat4(1.0f);

  Log::getDebug().log("Creating scene object: sceneModel.");

  Model m;
  m.setDisabled(false);
  Command c;
  auto metadata = ProgramState::getInstance().getMetadata();
  std::string vb_name;
  if (vb_name == "")
  {
     createBaseVertexBuffer();
     vb_name = metadata->base_vertex_buffer_name;
  }
  else {
    vb_name = metadata->base_vertex_buffer_name;
    VertexBufferManager::getInstance().deleteVertexBuffer(vb_name);
    createBaseVertexBuffer();
  }
  Vertexbuffer* vb = VertexBufferManager::getInstance().getVertexBufferByName(vb_name);
  c.vao = vb->getVAO(); 
  c.draw = GL_TRIANGLES;
  c.shaderName = metadata->meshShader;
  c.startIndex = 0;
  c.count = vb->getCount();
  c.modelMatrix = glm::mat4(1.0f);
  m.addCommand(c);
  auto modelData = std::make_tuple("sceneModel",m);
  pModels.push_back(modelData);
  return getModelByName("sceneModel");
}

const std::vector<Model*> ModelManager::getModels() const
{
  std::vector<Model*> result;
   for (const auto& data : pModels)
   {
     result.push_back(const_cast<Model*>(&std::get<1>(data)));
   }
   return result;
}

bool ModelManager::deleteModel(const std::string& name) {
   for (unsigned int i=0 ; i < pModels.size() ; i++)
   {
     if (std::get<0>(pModels[i]) == name)
     {
       auto ind = pModels.begin() + i;
       //std::get<1>(pModels[i]).dispose();
       pModels.erase(ind);
       return true;
     }
   }
   return false;
}
