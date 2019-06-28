#ifndef VERTEXBUFFERMANAGER_H
#define VERTEXBUFFERMANAGER_H

#include <vector>
#include <tuple>
#include <string>
#include <stdexcept>
#include "vertexbuffer.h"
#include "texture.h"
#include "textureManager.h"
#include "shader.h"
#include "manager.h"
#include "programstate.h"
#include "../Utils/log.h"
#include "../Utils/misc.h"

class Vertexbuffer;


class VertexBufferManager
{
  public:
    /// Creates and return a reference to the VertexBufferManager object.
    static VertexBufferManager& getInstance();
  
    Vertexbuffer* createVertexBuffer(const std::string& name);
    Vertexbuffer* getVertexBufferByName(const std::string& name);
    Vertexbuffer* optimize_vertex_buffer(const std::string& optimized_name, const std::string& shaderName);
    bool deleteVertexBuffer(const std::string& name);
  
  private:
    VertexBufferManager();
    ~VertexBufferManager();
  
    std::vector<std::tuple<std::string, Vertexbuffer>> pVertexBuffers;

};

#endif // VERTEXBUFFERMANAGER_H
