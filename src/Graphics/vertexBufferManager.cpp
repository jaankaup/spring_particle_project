#include "vertexBufferManager.h"

VertexBufferManager::VertexBufferManager()
{

}

VertexBufferManager::~VertexBufferManager()
{
   for (const auto& data : pVertexBuffers)
   {
     std::get<1>(data).dispose();
   }
}

Vertexbuffer* VertexBufferManager::createVertexBuffer(const std::string& name)
{
  Vertexbuffer s;
  s.init();
  auto vertexBufferData = std::make_tuple(name,s);
  pVertexBuffers.push_back(vertexBufferData);
  return getVertexBufferByName(name);
}

VertexBufferManager& VertexBufferManager::getInstance()
{
    static VertexBufferManager instance;
    return instance;
}

Vertexbuffer* VertexBufferManager::getVertexBufferByName(const std::string& name)
{
    for (const auto& obj : pVertexBuffers)
    {
        if (std::get<0>(obj) == name)
        {
            auto result = &(std::get<1>(obj));
            return const_cast<Vertexbuffer*>(result);
        }
    }
    throw std::runtime_error("VertexBufferManager::getVertexBufferByName(" + name + "). No such vertexBuffer.");
}

Vertexbuffer* VertexBufferManager::optimize_vertex_buffer(const std::string& optimized_name,
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
  Shader* shader = ShaderManager::getInstance().getShaderByName(metadata->triangulationShader);
  shader->bind();

  // Bind the tri_table.
  Texture tritable = TextureManager::getInstance().getTextureByName(metadata->tri_table_name);
  tritable.use(1);
  shader->setUniform("tri_table", 1);
  
  // Bind the 3D texture.
  Texture texture = TextureManager::getInstance().getTextureByName(metadata->texture3Dname);
  texture.use(0);

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
  Vertexbuffer* result = createVertexBuffer(optimized_name);
  result->init();
  std::vector<std::string> types = {"3f","3f"};
  result->addData(feedback, actual_data_size, types);
  result->pDataCount = primitiveCount*6;

  delete[] feedback;
  return result;
}

bool VertexBufferManager::deleteVertexBuffer(const std::string& name)
{
   for (unsigned int i=0 ; i < pVertexBuffers.size() ; i++)
   {
     if (std::get<0>(pVertexBuffers[i]) == name)
     {
       auto ind = pVertexBuffers.begin() + i;
       std::get<1>(pVertexBuffers[i]).dispose();
       pVertexBuffers.erase(ind);
       return true;
     }
   }
   return false;
}
