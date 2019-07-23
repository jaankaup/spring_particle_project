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
    glPointSize(3);
    glLineWidth(3);
}

void Renderer::render(const Camera& camera) {
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.001f, 1000.0f);

  glm::mat4 MVP = projection * camera.getMatrix() * glm::mat4(1.0f);  

  /* Verhon piirto. */

  if (ProgramState::getInstance().getShowVerho()) {
    ProgramState::getInstance().setTimeStep(0.0008f);
    auto verho_particle_system = ParticleSystemManager::getInstance().getByKey("verho");
    verho_particle_system->takeStep(ProgramState::getInstance().getTimeStep());
    verho_particle_system->draw(MVP);
  }

  /* Lumen piirto. */

  if (ProgramState::getInstance().getShowLumi()) {
    ProgramState::getInstance().setTimeStep(0.0002f);
    auto lumi_particle_system = ParticleSystemManager::getInstance().getByKey("lumi");
    lumi_particle_system->takeStep(ProgramState::getInstance().getTimeStep());
    lumi_particle_system->draw(MVP);
    ProgramState::getInstance().setTimeStep(0.0008f);
  }


  /* Ruohikon piirto. */

  if (ProgramState::getInstance().getShowRuohikko()) {

      // Ei piirreta maata jos tuulivektorin nakyvissa. Se hiena haittaa
      // nakyvyytta.
      if (!ProgramState::getInstance().getShowWind()) {
        auto render_shader_name = ProgramState::getInstance().getMetadata()->meshShader;
        Shader* shader = ShaderManager::getInstance().getByKey(render_shader_name);
        shader->bind();
        shader->setUniform("MVP", MVP);
        shader->setUniform("input_color", glm::vec3(0.7f,0.3f,1.0f));
        auto mua = VertexBufferManager::getInstance().getByKey("maa_pisteet");
        mua->bind();
        glDrawArrays(GL_POINTS, 0, 40*40);
      }

      //ProgramState::getInstance().setTimeStep(0.0012f);
      auto ruohikko_particle_system = ParticleSystemManager::getInstance().getByKey("ruohikko");
      ruohikko_particle_system->takeStep(ProgramState::getInstance().getTimeStep());
      ruohikko_particle_system->draw(MVP);
      ProgramState::getInstance().setTimeStep(0.0008f);
    

  }

  /* Tuuli vektorien piirto. */
  
  if (ProgramState::getInstance().getShowWind()) {
    auto tuuliShader = ShaderManager::getInstance().getByKey("tuulishader");
    tuuliShader->bind();

    tuuliShader->setUniform("MVP", MVP);
    auto metadata = ProgramState::getInstance().getMetadata();
    Texture* texture = TextureManager::getInstance().getByKey(metadata->texture3Dname);
    texture->use(0);
    tuuliShader->setUniform("diffuse3DTexture",0);
    tuuliShader->setUniform("time",ProgramState::getInstance().get_h_sum());

    auto tuuliPoints = VertexBufferManager::getInstance().getByKey("tuuli_pisteet");
    tuuliPoints->bind();
    auto tuuliCount = tuuliPoints->getCount();

    glDisable(GL_CULL_FACE);
    glDrawArrays(GL_POINTS, 0, tuuliCount);
    glEnable(GL_CULL_FACE);
  }
}

void Renderer::renderModels(const Camera& camera)
{
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glm::mat4 projection = glm::perspective(glm::radians(45.0f), 16.0f / 9.0f, 0.001f, 1000.0f);

  glm::vec3 eyePosition = camera.getPosition();
  glm::mat4 viewMatrix = camera.getMatrix();

  auto models = ModelManager::getInstance().getModels();
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
