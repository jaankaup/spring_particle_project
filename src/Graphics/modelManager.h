#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <vector>
#include <tuple>
#include <stdexcept>
#include "model.h"
#include "texture.h"
#include "textureManager.h"
#include "vertexbuffer.h"
#include "vertexBufferManager.h"
#include "../Utils/log.h"
#include "../Utils/misc.h"

class Model;

class ModelManager
{
  public:
    /// Creates and return a reference to the ModelManager object.
    static ModelManager& getInstance();
  
    Model* createModel(const std::string& name);
    Model* getModelByName(const std::string& name) const;

    const std::vector<Model*> getModels() const;

    Model* createSceneObject();

    bool deleteModel(const std::string& name); 
  
  private:
    ModelManager();
    ~ModelManager();
  
    /// Name:Model pairs.
    std::vector<std::tuple<std::string, Model>> pModels;
};

#endif // MODELMANAGER_H
