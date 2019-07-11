#include "renderer.h"
#include "window.h"
Renderer::Renderer()
{
}
	
Renderer::~Renderer()
{

}

void Renderer::init()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(8);
}

void Renderer::renderModels(const Camera& camera)
{
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glm::vec3 eyePosition = camera.getPosition();
  glm::mat4 viewMatrix = camera.getMatrix();

  auto models = ModelManager::getInstance().getModels();
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.001f, 1000.0f);
  std::vector<Command> commands;
  for (const auto m : models)
  {
    if (m->getDisabled()) continue;
    for (const auto& c : *m->getCommands())
    {
      commands.push_back(c);
    }
  }

    float voxels_per_block = ProgramState::getInstance().getVoxelsPerBlock();
  for (const auto& com : commands)
  {
    Shader* shader = ShaderManager::getInstance().getByKey(com.shaderName);
    shader->bind();
    shader->setUniform("voxels_per_block", voxels_per_block);

    auto metadata = ProgramState::getInstance().getMetadata();
    auto tName = metadata->texture3Dname;
    glm::mat4 mx = com.modelMatrix ;
    // TODO::: MODIFY ::: TODO
    //Texture texture = TextureManager::getInstance().getTextureByName(tName);//{TextureType::d2,0};
    Texture* texture = TextureManager::getInstance().getByKey(tName);//{TextureType::d2,0};
    texture->use(0);
    glBindVertexArray(com.vao);

    // TODO: add lights only once.
    shader->setUniform("MVP", projection * viewMatrix * mx);
    shader->setUniform("normalMatrix", glm::inverseTranspose(glm::mat3(mx)));
    shader->setUniform("M", mx);
    shader->setUniform("lights[0].color", glm::vec3(1.0f,1.0f,1.0f));
    shader->setUniform("lights[0].ambientCoeffience", 0.25f);
    shader->setUniform("lights[0].materialSpecularColor", glm::vec3(1.0f,1.0f,1.0f));
    shader->setUniform("lights[0].materialShininess", 70.0f);
    shader->setUniform("lights[0].attentuationFactor", 0.00009f);
    shader->setUniform("cameraPosition", eyePosition);
    shader->setUniform("lights[0].position", glm::vec3(8.0f,8.0f,8.0f));/* eyePosition);*/

    // TODO::: MODIFY ::: TODO
    //Texture tritable = TextureManager::getInstance().getTextureByName("tri_table_texture");//{TextureType::d2,0};
    Texture* tritable = TextureManager::getInstance().getByKey("tri_table_texture");//{TextureType::d2,0};

    // TODO: wireframe is not available in this version. Remove these lines.
    if (com.shaderName == "marchingShaderLine" && !ProgramState::getInstance().getWireframe()) continue; 
    switch (texture->getTextureType())
    {
      case TextureType::d1:
        // Do nothing.
        break;
      case TextureType::d2:
        shader->setUniform("diffuseTexture",0);
        break;
      case TextureType::d3:
        shader->setUniform("diffuse3DTexture",0);
        break;
    }
    tritable->use(1);
    shader->setUniform("tri_table", 1);

    switch (com.draw)
    {
      case GL_TRIANGLES:
        glDrawArrays(GL_TRIANGLES, com.startIndex, com.count);
        break;
      case GL_POINTS:
        glDrawArrays(GL_POINTS, com.startIndex, com.count);
        break;
    }
  }
}

void Renderer::renderKipinat(const Camera& camera)
{
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glm::vec3 eyePosition = camera.getPosition();
  glm::mat4 viewMatrix = camera.getMatrix();
  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.001f, 1000.0f);
  glm::mat4 mx = glm::mat4(1.0f);
  auto render_shader_name = ProgramState::getInstance().getMetadata()->meshShader;
  auto bState_name = ProgramState::getInstance().getMetadata()->baseState;
  ParticleBuffer* vb = dynamic_cast<ParticleBuffer*>(VertexBufferManager::getInstance().getByKey(bState_name));
  //if (vb == nullptr) {
  //  Log::getInfo().log("NULLLLIIIIII");
  //}
  //else
  //{
  //  Log::getInfo().log("EI NULLLLIIIIII");
  //}
  vb->takeStep(0.002f);
  vb->bind();
  //Log::getInfo().log("BINDAUS ONNISTU");

///  //vb->bind();
///  unsigned int euler;
///  //unsigned int vao;
///  glGenBuffers(1,&euler);
///  //glVertexArrays(1,&vao);
///  //glBindVertexArray(vao);
///  glBindBuffer(GL_ARRAY_BUFFER, euler);
///  //glBindBuffer(GL_ARRAY_BUFFER, vb-getHandle());
///  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, euler); // TODO continue
///  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vb->getHandle()); // TODO continue
///
///  Shader* compute = ShaderManager::getInstance().getByKey("particle1");
///  compute->bind();
///
///  glDispatchCompute(1,1,1);
///
///  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
  
  //Log::getInfo().log("HIHHIIIIII 2");
  Shader* shader = ShaderManager::getInstance().getByKey(render_shader_name);
  //Log::getInfo().log("HIHHIIIIII 3");
  shader->bind();
  //Log::getInfo().log("HIHHIIIIII 4");
  shader->setUniform("MVP", projection * viewMatrix * mx);
  //Log::getInfo().log("HIHHIIIIII 5");
  auto particleCount = ProgramState::getInstance().getParticleCount();
  glDrawArrays(GL_POINTS, 0, particleCount);
  //Log::getInfo().log("HIHHIIIIII");
}
