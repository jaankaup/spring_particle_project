#ifndef MANAGER_H
#define MANAGER_H

#include <vector>
#include <iostream>
//#include <unordered_map>
#include <stdexcept>
#include <memory>
#include "shader.h"
#include "texture.h"
#include "vertexbuffer.h"
//#include "../Utils/misc.h"
//#include <string>
//#include "textureManager.h"
#include "programstate.h"
#include "../Utils/log.h"
#include "../Utils/misc.h"
#include "../Physics/ParticleSystem.h"

/* Forward declarations. */
class Shader;
class Texture;
class Vertexbuffer;
class ParticleSystem;
class VerhoSystem;

enum class ParticleType { Verho };

/***************************************************************************************** 
 *                                                                                       *  
 *  Key-value struct.                                                                    *  
 *                                                                                       *  
 ****************************************************************************************/

template<typename T>
class Element {
  public:
    std::string key;
    std::unique_ptr<T> val;

    Element(const std::string& key, std::unique_ptr<T> val) : key{key}, val{std::move(val)} {}
};

/****************************************************************************************
 *                                                                                      *  
 * A base (singleton) class for storing key/value pairs. T is the type of the stored    *
 * values.                                                                              *
 *                                                                                      *  
 ****************************************************************************************/

template<typename T>
class Manager
{

  public:

    /// Copying and moving Manager is forbidden (sigleton class).
    /// Does children derive these???
    Manager(const Manager&) = delete;
    Manager(Manager&&) = delete;
    Manager& operator=(const Manager&) = delete;
    Manager& operator=(Manager&&) = delete;

    /// Creates and return a reference to the singleton Manager object.
    static auto& getInstance();

    /// Create resource (default).
    virtual T* create(const std::string& key);

    /// Get the resource using a string key. Return pointer to the 
    /// created resource. Otherwise returns nullptr.
    virtual T* getByKey(const std::string& key) const;

    /// Deletes an element with a string key. Returns false if deletion fails. 
    /// Otherwise return true.
    virtual bool del(const std::string& key);
  
    /// For some reason the destructor must be public.
    virtual ~Manager() {};

  protected:

    /// Default constructor.
    Manager() {};
    std::vector<Element<T>> pData;

  private:

};

template<typename T>
inline auto& Manager<T>::getInstance()
{
  static Manager instance;
  return instance;
}

template<typename T>
inline T* Manager<T>::create(const std::string& key)
{
  /* Check if the already existst. */
  if (getByKey(key) != nullptr)
  {
    del(key);
    // TODO: log to debug.
    //return nullptr; 
  }
  Log::getInfo().log("Manager::create(%):: Luodaan resurssi. ", key);

  Element<T> e(key,std::unique_ptr<T>(new T()));
  auto ret_val = e.val.get();
  pData.push_back(std::move(e));
  return ret_val;
}

template<typename T>
inline T* Manager<T>::getByKey(const std::string& key) const
{
  for (const auto& e : pData)
  {
    if (e.key == key)
    {
      //Log::getInfo().log("getByKey: %",key);
      //e.val.get()->bind();
      return e.val.get();
    }
  }
  Log::getInfo().log("Manager::getByKey(%):: Ei loytynyt resurssia. ", key);
  return nullptr;
}

template<typename T>
inline bool Manager<T>::del(const std::string& key)
{
  for (int i=0; i<pData.size() ; i++)
  {
    if (pData[i].key == key)
    {
      Log::getInfo().log("Deleting %", key);
      pData.erase(pData.begin() + i);
      return true;
    }
  }
  Log::getInfo().log("Not deleting %", key);
  return false;
}

/****************************************************************************************
 *                                                                                      *  
 * Shader manager                                                                       *
 *                                                                                      *  
 ****************************************************************************************/

class ShaderManager : public Manager<Shader>
{
  public:

    // Get the singleton instance of this class.
    static auto& getInstance() { static ShaderManager instance; return instance; }

};

/****************************************************************************************
 *                                                                                      *  
 * Texture manager                                                                      *
 *                                                                                      *  
 ****************************************************************************************/

class TextureManager : public Manager<Texture>
{

  public:

    // Get the singleton instance of this class.
    static auto& getInstance() { static TextureManager instance; return instance; }

      //Log::getInfo().log("getByKey: %",key);
    Texture* create(const std::string& key) override { Log::getInfo().log("TextureManager::create(): not implemented."); return nullptr; }

    Texture* create(const std::string& key, const TextureType& type);
//    {
//      auto ret_val = Manager<Texture>::create(key); 
//      ret_val->init(type);
//      return ret_val;
//    }

};

/****************************************************************************************
 *                                                                                      *  
 * VertexBuffer manager                                                                 *
 *                                                                                      *  
 ****************************************************************************************/

class VertexBufferManager : public Manager<Vertexbuffer>
{

  public:

    // Get the singleton instance of this class.
    static auto& getInstance() { static VertexBufferManager instance; return instance; }

    // TODO: shaderName is not used. Remove it.
    Vertexbuffer* optimize_vertex_buffer(const std::string& optimized_name, const std::string& shaderName);

    Vertexbuffer* createParticleBuffer(const std::string& name);

};

/****************************************************************************************
 *                                                                                      *  
 * ParticleSystemManager                                                                *
 *                                                                                      *  
 ****************************************************************************************/

class ParticleSystemManager : public Manager<ParticleSystem>
{

  public:

    // Get the singleton instance of this class.
    static auto& getInstance() { static ParticleSystemManager instance; return instance; }

    ParticleSystem* create(const std::string& key, const ParticleType type); 
};

inline Texture* TextureManager::create(const std::string& key, const TextureType& type)
{
  auto ret_val = Manager<Texture>::create(key); 
  ret_val->init(type);
  return ret_val;
}

inline Vertexbuffer* VertexBufferManager::optimize_vertex_buffer(const std::string& optimized_name,
                                                          const std::string& shaderName)
{
  
  auto metadata = ProgramState::getInstance().getMetadata();
  int activeShader = metadata->activeShaderNumber;
  auto dimensions = metadata->dimensionsPerDF[activeShader];

  // Create cube base points (front left point of cube) for a single cube.
  int blockSize = metadata->block_size; 
  int size = blockSize*blockSize*blockSize; 
  std::vector<float> points;
  for (int i=0 ; i<blockSize ; ++i) {
  for (int j=0 ; j<blockSize ; ++j) {
  for (int k=0 ; k<blockSize ; ++k) {
    points.push_back((float)i);
    points.push_back((float)j);
    points.push_back((float)k);
  }}};

  // Create the "dimension" of the scene.
  // 8^3 cubes. 
  std::vector<glm::vec3> basePositions;
  for (int i = std::get<0>(dimensions) ; i < std::get<3>(dimensions) ; i++) {
  for (int j = std::get<1>(dimensions) ; j < std::get<4>(dimensions) ; j++) {
  for (int k = std::get<2>(dimensions) ; k < std::get<5>(dimensions) ; k++) {
    basePositions.push_back(glm::vec3(float(i)*blockSize,float(j)*blockSize,float(k)*blockSize));
  }}};

  uint32_t cubeCount = basePositions.size()*size;
  Log::getInfo().log("Marching % cubes...", std::to_string(cubeCount));

  // Create instanced data.
  unsigned int instanceVBO;
  glGenBuffers(1, &instanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * basePositions.size(), &basePositions[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Create the base points data and create data bindings for triangulate.vert
  // shader.
  unsigned int quadVAO, quadVBO;
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float)*points.size(), &points[0], GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  // also set instance data
  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); // this attribute comes from a different vertex buffer
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glVertexAttribDivisor(1, 1); // tell OpenGL this is an instanced vertex attribute.

  // Find the trinagulate shader.
  Log::getInfo().log("Etsitaan triandulation shaderi");
  Shader* shader = ShaderManager::getInstance().getByKey(metadata->triangulationShader);
  shader->bind();

  // Bind the tri_table.
  Texture* tritable = TextureManager::getInstance().getByKey(metadata->tri_table_name);
  tritable->use(1);
  shader->setUniform("tri_table", 1);
  
  // Bind the 3D texture.
  Texture* texture = TextureManager::getInstance().getByKey(metadata->texture3Dname);
  texture->use(0);

  shader->setUniform("diffuse3DTexture",0);
  shader->setUniform("voxels_per_block", (float)metadata->block_size);

  // This is a hard code size for the feed back buffer. This might not work in
  // all graphics cards.
  auto transformFeedbackCount = 500000000;

  // Create the feedback buffer.
  GLuint tbo;
  glGenBuffers(1, &tbo);
  glBindBuffer(GL_ARRAY_BUFFER, tbo);
  glBufferData(GL_ARRAY_BUFFER, transformFeedbackCount , nullptr, GL_STATIC_READ);

  // We don't want the clipping stage.
  glEnable(GL_RASTERIZER_DISCARD);

  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tbo);

  // The counter for the output count of geometry shader.
  GLuint query;
  glGenQueries(1,&query);

  // Start the counter.
  glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN,query);

  // Perform feedback.
  glBeginTransformFeedback(GL_TRIANGLES);
  glDrawArraysInstanced(GL_POINTS, 0, size, basePositions.size());
  glEndTransformFeedback();

  // :noremap <F12> :wall \| !./native_compile.sh && ./runNative.sh<CR>

  // Flush everythin out of geometry shader.
  glFlush();

  // We can now enable the rasterizer.
  glDisable(GL_RASTERIZER_DISCARD);

  // Stop the counter.
  glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

  // The result of the counter.
  GLuint primitiveCount;
  glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitiveCount);

  // Take the result of geometry shader to 'feecback' buffer.
  unsigned int actual_data_size = primitiveCount*6*sizeof(float)*3;
  GLfloat* feedback = new GLfloat[primitiveCount*6*3];
  glGetBufferSubData(GL_ARRAY_BUFFER, 0, actual_data_size, feedback);

  Log::getInfo().log("Generated % triangles...", std::to_string(primitiveCount));

  // Clean up the buffers.
  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &tbo);
  glDeleteBuffers(1, &instanceVBO);
  glDeleteBuffers(1, &quadVBO);

  // Create the vertexbuffer from the geometry shader output data.
  Vertexbuffer* result = Manager<Vertexbuffer>::create(optimized_name);
  result->init();
  std::vector<std::string> types = {"3f","3f"};
  result->addData(feedback, actual_data_size, types);
  result->pDataCount = primitiveCount*6;

  delete[] feedback;
  return result;
}

inline Vertexbuffer* VertexBufferManager::createParticleBuffer(const std::string& key)
{
  /* Check if the already existst. */
  if (getByKey(key) != nullptr)
  {
    del(key);
  }

  Element<Vertexbuffer> e(key,std::unique_ptr<Vertexbuffer>(new ParticleBuffer()));
  auto ret_val = e.val.get();
  pData.push_back(std::move(e));
  return ret_val;
}

inline ParticleSystem* ParticleSystemManager::create(const std::string& key, const ParticleType type)
{
  if (getByKey(key) != nullptr) del(key);

  if (type == ParticleType::Verho) {
    Element<ParticleSystem> e(key,std::unique_ptr<ParticleSystem>(new VerhoSystem()));
    auto ret_val = e.val.get();
    pData.push_back(std::move(e));
    return ret_val;
  }
  else {
    Element<ParticleSystem> e(key,std::unique_ptr<ParticleSystem>(new ParticleSystem()));
    auto ret_val = e.val.get();
    pData.push_back(std::move(e));
    return ret_val;
  }
}

#endif // MODELMANAGER_H
