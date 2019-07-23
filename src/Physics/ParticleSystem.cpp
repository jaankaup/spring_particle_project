#include "ParticleSystem.h"
#include "../Graphics/vertexbuffer.h"
#include "../Graphics/programstate.h"
#include "../Graphics/manager.h"
#include "../Utils/misc.h"
#include "../Utils/log.h"
#include "../Utils/myrandom.h"
#include "ParticleGenerator.h"

const std::string VerhoSystem::INITIAL_BUFFER = "verho_initial";
const std::string VerhoSystem::STATIC_DATA_BUFFER = "verho_static";
const std::string VerhoSystem::K1 = "verho_k1";
const std::string VerhoSystem::K2 = "verho_k2";
const std::string VerhoSystem::K3 = "verho_k3";
const std::string VerhoSystem::K4 = "verho_k4";
const std::string VerhoSystem::CS_NAME = "verho_shader";

const std::string LumihiutaleSystem::INITIAL_BUFFER = "lumi_initial";
const std::string LumihiutaleSystem::STATIC_DATA_BUFFER = "lumi_static";
const std::string LumihiutaleSystem::K1 = "lumi_k1";
const std::string LumihiutaleSystem::K2 = "lumi_k2";
const std::string LumihiutaleSystem::K3 = "lumi_k3";
const std::string LumihiutaleSystem::K4 = "lumi_k4";
const std::string LumihiutaleSystem::CS_NAME = "lumi_shader";

const std::string RuohikkoSystem::INITIAL_BUFFER = "ruohikko_initial";
const std::string RuohikkoSystem::STATIC_DATA_BUFFER = "ruohikko_static";
const std::string RuohikkoSystem::K1 = "ruohikko_k1";
const std::string RuohikkoSystem::K2 = "ruohikko_k2";
const std::string RuohikkoSystem::K3 = "ruohikko_k3";
const std::string RuohikkoSystem::K4 = "ruohikko_k4";
const std::string RuohikkoSystem::CS_NAME = "ruohikko_shader";

uint32_t ParticleSystem::get_particle_count() const
{
  return pParticleCount;
}

void ParticleSystem::init()
{
  Log::getDebug().log("ParticleSystem::init");
}

/************************************************************
 *
 *  VerhoSystem
 *
 ***********************************************************/

void VerhoSystem::init()
{
  Log::getDebug().log("VerhoSystem::init");
  //auto ps = ProgramState::getInstance(); 

  // Luodaan tarvittavat puskurit.

  // Piirrettava data.
  auto initial_data = VertexBufferManager::getInstance().create(VerhoSystem::INITIAL_BUFFER);
  initial_data->init(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW);

  // Staattinen data, partikkelin kaveri-idt, lepopituudet ym.
  auto static_data = VertexBufferManager::getInstance().create(VerhoSystem::STATIC_DATA_BUFFER);
  static_data->init_plain_buffer(GL_ARRAY_BUFFER,GL_STATIC_DRAW);

  // k1-k4 valiaikaispuskurit RK4:sta varten.
  auto k1 = VertexBufferManager::getInstance().create(VerhoSystem::K1);
  k1->init_plain_buffer(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW);

  auto k2 = VertexBufferManager::getInstance().create(VerhoSystem::K2);
  k2->init_plain_buffer(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW);

  auto k3 = VertexBufferManager::getInstance().create(VerhoSystem::K3);
  k3->init_plain_buffer(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW);

  auto k4 = VertexBufferManager::getInstance().create(VerhoSystem::K4);
  k4->init_plain_buffer(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW);

  // Luodaan compute shader, jos sita ei ole entuudestaan luotu.
  auto compute_shader = ShaderManager::getInstance().getByKey(VerhoSystem::CS_NAME);
  if (compute_shader == nullptr) compute_shader = ShaderManager::getInstance().create(VerhoSystem::CS_NAME);
  std::vector<std::string> src = {"shaders/jouset.comp"};
  compute_shader->build(src,false);

  const int width = 100;
  const int height = 100;
  pParticleCount = width*height;
  float offset = 0.005f;

  glm::vec4 basePos = glm::vec4(0.0f,0.5f,0.0f,1.0f);
  glm::vec3 a = glm::vec3(0.0f);
  glm::vec3 b = glm::vec3(offset,offset,0.0f);
  glm::vec3 c = glm::vec3(offset,0.0f,0.0f);

  float dist = glm::distance(a,b);
  float dist_horizontal = glm::distance(a,c);

  auto array = new GLfloat[8*pParticleCount];

  auto static_data_array = new GLfloat[20*pParticleCount];

  // Luodaan data.
  for (int j=0 ; j<width ; j++) {
  for (int i=0 ; i<height ; i++) {
    int position = i + width * j;
    int pah = position*8; 
    int static_pah = position*20; 

    // Partikkelin positio.
    array[pah] = (i%width)*offset + basePos.x; 
    array[pah+1] = (-1.0f)*j*offset + basePos.y; 
    array[pah+2] = 0.0f + basePos.z; 
    array[pah+3] = 1.0f; 

    // Alku velocity partikkelille.
    array[pah+4] = 0.0f; 
    array[pah+5] = 0.0f; 
    array[pah+6] = 0.0f; 
    array[pah+7] = 0.0f; 

    // Jos on ylhaalla, niin asetetaan arvoksi -1.0f, sovitaan etta on
    // staatinen partikkeli. Debuggaus mielessa muuten 6.0f.
    static_data_array[static_pah] = getUp(position,width,height) == -1 ? -1.0f : 6.0f; 
    static_data_array[static_pah+1] = 0.0f; // Tanne jotain muuta informaatiota jos keksii.  
    static_data_array[static_pah+2] = 0.0f; // Tanne jotain muuta informaatiota jos keksii.
    static_data_array[static_pah+3] = 0.0f; // Tanne jotain muuta informaatiota jos keksii.

    // Kavereiden indeksit. Yla-ala kulmat..
    float ur = float(getUpRight(position,width,height)); 
    float ul = float(getUpLeft(position,width,height)); 
    float br = float(getBottomRight(position,width,height)); 
    float bl = float(getBottomLeft(position,width,height)); 
    static_data_array[static_pah+4] = ur; 
    static_data_array[static_pah+5] = ul;  
    static_data_array[static_pah+6] = br;
    static_data_array[static_pah+7] = bl;

    // Kavereiden indeksit. Sivut..
    float u = float(getUp(position,width,height)); 
    float l = float(getLeft(position,width,height)); 
    float b = float(getBottom(position,width,height)); 
    float r = float(getRight(position,width,height)); 
    static_data_array[static_pah+8] = u; 
    static_data_array[static_pah+9] = l;  
    static_data_array[static_pah+10] = b;
    static_data_array[static_pah+11] = r;

    // Tanne lepoarvoja kavereihin. Nyt on kaikilla sama, mutta jos olisi
    // erilainen jousirakenne, niin tanne voisi laittaa muitakin arvoja.
    static_data_array[static_pah+12] = dist;
    static_data_array[static_pah+13] = dist;  
    static_data_array[static_pah+14] = dist;
    static_data_array[static_pah+15] = dist;
    static_data_array[static_pah+16] = dist_horizontal;
    static_data_array[static_pah+17] = dist_horizontal;  
    static_data_array[static_pah+18] = dist_horizontal;
    static_data_array[static_pah+19] = dist_horizontal;
  }}; 

  // Taytetaan puskurit. Piirtopuskuri, on samalla partikkeleiden tila.
  std::vector<std::string> types = {"4f","4f"};
  initial_data->addData(array,sizeof(float)*pParticleCount*8,types);
  initial_data->setCount(pParticleCount);

  // Staattisen datan puskuri.
  static_data->populate_data(static_data_array,sizeof(float)*pParticleCount*20);

  // k1-k4 puskurit. Varataan vain tila. Ei tarvitse siirtaa dataa.
  k1->populate_data(nullptr,sizeof(float)*pParticleCount*4);
  k2->populate_data(nullptr,sizeof(float)*pParticleCount*4);
  k3->populate_data(nullptr,sizeof(float)*pParticleCount*4);
  k4->populate_data(nullptr,sizeof(float)*pParticleCount*4);
  delete[] array;
  delete[] static_data_array;

//  // Luodaan data verhon piirtoa varten.
//  auto piirto_indeksit = VertexBufferManager::getInstance().create("piirto_indeksit_verho");
//  piirto_indeksit->init();
//  std::vector<std::string> types_indeksit = {"1f"};
//
//  auto array_ind = new GLfloat[pParticleCount];
//  for (int i=0; i<pParticleCount; i++)
//  {
//    array_ind[i] = float(i);
//  }
//  piirto_indeksit->addData(array_ind,sizeof(float)*(pParticleCount),types_indeksit);
//  piirto_indeksit->setCount(pParticleCount);
//  delete[] array_ind;
}

void VerhoSystem::takeStep(const float h)
{
  auto cs = ShaderManager::getInstance().getByKey(VerhoSystem::CS_NAME);
  cs->bind();

  auto metadata = ProgramState::getInstance().getMetadata();
  Texture* texture = TextureManager::getInstance().getByKey(metadata->texture3Dname);
  texture->use(0);
  cs->setUniform("diffuse3DTexture",0);

  auto wind_strength = ProgramState::getInstance().getWindStrength();

  cs->setUniform("h",ProgramState::getInstance().getTimeStep());
  cs->setUniform("time",ProgramState::getInstance().get_h_sum());
  cs->setUniform("wind_strength", ProgramState::getInstance().getStopWind() ? 0.0f : wind_strength);

  auto initial_data = VertexBufferManager::getInstance().getByKey(VerhoSystem::INITIAL_BUFFER);
  auto static_data = VertexBufferManager::getInstance().getByKey(VerhoSystem::STATIC_DATA_BUFFER);
  auto k1 = VertexBufferManager::getInstance().getByKey(VerhoSystem::K1);
  auto k2 = VertexBufferManager::getInstance().getByKey(VerhoSystem::K2);
  auto k3 = VertexBufferManager::getInstance().getByKey(VerhoSystem::K3);
  auto k4 = VertexBufferManager::getInstance().getByKey(VerhoSystem::K4);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, initial_data->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, static_data->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, k1->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, k2->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, k3->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, k4->getHandle());

  const static int X = 100;
  const static int Y = 1;

  // K1
  cs->setUniform("phase",1.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // K2
  cs->setUniform("phase",2.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // K3
  cs->setUniform("phase",3.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // K4
  cs->setUniform("phase",4.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // Lopullinen tulos.
  cs->setUniform("phase",5.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void VerhoSystem::draw(const glm::mat4& mvp)
{

  auto drawBuffer = VertexBufferManager::getInstance().getByKey(VerhoSystem::INITIAL_BUFFER); 

  drawBuffer->bind();

  auto render_shader_name = ProgramState::getInstance().getMetadata()->meshShader;
  Shader* shader = ShaderManager::getInstance().getByKey(render_shader_name);
  shader->bind();
  shader->setUniform("MVP", mvp);
  //shader->setUniform("input_color", glm::vec3(0.2f,0.0f,1.0f));
  shader->setUniform("input_color", glm::vec3(0.9f,0.9f,0.9f));

  glDrawArrays(GL_POINTS, 0, pParticleCount);
}

/************************************************************
 *
 *  LumihiutaleSystem
 *
 ***********************************************************/

void LumihiutaleSystem::init()
{
  Log::getDebug().log("LumihiutaleSystem::init");
  //auto ps = ProgramState::getInstance(); 

  // Luodaan tarvittavat puskurit.

  // Piirrettava data.
  auto initial_data = VertexBufferManager::getInstance().create(LumihiutaleSystem::INITIAL_BUFFER);
  initial_data->init(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW);

  // Staattinen data. Nyt ei ole viela keksitty mita tanne laittaa.
  auto static_data = VertexBufferManager::getInstance().create(LumihiutaleSystem::STATIC_DATA_BUFFER);
  static_data->init_plain_buffer(GL_ARRAY_BUFFER,GL_STATIC_DRAW);

  // k1-k4 valiaikaispuskurit RK4:sta varten.
  auto k1 = VertexBufferManager::getInstance().create(LumihiutaleSystem::K1);
  k1->init_plain_buffer(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW);

  auto k2 = VertexBufferManager::getInstance().create(LumihiutaleSystem::K2);
  k2->init_plain_buffer(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW);

  auto k3 = VertexBufferManager::getInstance().create(LumihiutaleSystem::K3);
  k3->init_plain_buffer(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW);

  auto k4 = VertexBufferManager::getInstance().create(LumihiutaleSystem::K4);
  k4->init_plain_buffer(GL_ARRAY_BUFFER,GL_DYNAMIC_DRAW);

  // Luodaan compute shader, jos sita ei ole entuudestaan luotu.
  auto compute_shader = ShaderManager::getInstance().getByKey(LumihiutaleSystem::CS_NAME);
  if (compute_shader == nullptr) compute_shader = ShaderManager::getInstance().create(LumihiutaleSystem::CS_NAME);
  std::vector<std::string> src = {"shaders/lumi.comp"};
  compute_shader->build(src,false);

  const int width = 60;
  const int height = 60;
  pParticleCount = width*height;

  auto array = new GLfloat[8*pParticleCount];

  auto static_data_array = new GLfloat[4*pParticleCount];

  // Luodaan data.
  for (int j=0 ; j<width ; j++) {
  for (int i=0 ; i<height ; i++) {
    int position = i + width * j;
    int pah = position*8; 
    int static_pah = position*4; 

//  VertexBufferManager::getInstance().createExamplePoints(30, 30, 30,30.0, glm::vec3(-0.5f,0.0f,-0.5f), "tuuli_pisteet");
    // Arvotaan lumihiutaleiden alkupaikat.
    MyRandom<float> mr_x;
    mr_x.setDistribution(-0.5f,0.5f);

    MyRandom<float> mr_y(std::to_string(mr_x()));
    mr_y.setDistribution(0.0f,1.0f);

    MyRandom<float> mr_z(std::to_string(mr_x()));
    mr_z.setDistribution(-0.5f,0.5f);

    // Partikkelin positio.
    array[pah] = mr_x(); 
    array[pah+1] = mr_y(); 
    array[pah+2] = mr_z(); 
    array[pah+3] = 1.0f; 

    // Alku velocity partikkelille.
    array[pah+4] = 0.0f; 
    array[pah+5] = 0.0f; 
    array[pah+6] = 0.0f; 
    array[pah+7] = 0.0f; 

    // Staattinen data lumihiutaleelle. Tanne jotain jos keksii.
    static_data_array[static_pah] = 0.0f; 
    static_data_array[static_pah+1] = 0.0f; 
    static_data_array[static_pah+2] = 0.0f; 
    static_data_array[static_pah+3] = 0.0f; 
  }}; 

  // Taytetaan puskurit. Piirtopuskuri, on samalla partikkeleiden tila.
  std::vector<std::string> types = {"4f","4f"};
  initial_data->addData(array,sizeof(float)*pParticleCount*8,types);
  initial_data->setCount(pParticleCount);

  // Staattisen datan puskuri.
  static_data->populate_data(static_data_array,sizeof(float)*pParticleCount*4);

  // k1-k4 puskurit. Varataan vain tila. Ei tarvitse siirtaa dataa.
  k1->populate_data(nullptr,sizeof(float)*pParticleCount*4);
  k2->populate_data(nullptr,sizeof(float)*pParticleCount*4);
  k3->populate_data(nullptr,sizeof(float)*pParticleCount*4);
  k4->populate_data(nullptr,sizeof(float)*pParticleCount*4);
  delete[] array;
  delete[] static_data_array;
}

void LumihiutaleSystem::takeStep(const float h)
{
  auto cs = ShaderManager::getInstance().getByKey(LumihiutaleSystem::CS_NAME);
  cs->bind();

  auto metadata = ProgramState::getInstance().getMetadata();
  Texture* texture = TextureManager::getInstance().getByKey(metadata->texture3Dname);
  texture->use(0);
  cs->setUniform("diffuse3DTexture",0);

  auto wind_strength = ProgramState::getInstance().getWindStrength();

  cs->setUniform("h",ProgramState::getInstance().getTimeStep());
  cs->setUniform("time",ProgramState::getInstance().get_h_sum());
  cs->setUniform("wind_strength", ProgramState::getInstance().getStopWind() ? 0.0f : wind_strength);

  auto initial_data = VertexBufferManager::getInstance().getByKey(LumihiutaleSystem::INITIAL_BUFFER);
  auto static_data = VertexBufferManager::getInstance().getByKey(LumihiutaleSystem::STATIC_DATA_BUFFER);
  auto k1 = VertexBufferManager::getInstance().getByKey(LumihiutaleSystem::K1);
  auto k2 = VertexBufferManager::getInstance().getByKey(LumihiutaleSystem::K2);
  auto k3 = VertexBufferManager::getInstance().getByKey(LumihiutaleSystem::K3);
  auto k4 = VertexBufferManager::getInstance().getByKey(LumihiutaleSystem::K4);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, initial_data->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, static_data->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, k1->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, k2->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, k3->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, k4->getHandle());

  const static int X = 60;
  const static int Y = 1;

  // K1
  cs->setUniform("phase",1.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // K2
  cs->setUniform("phase",2.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // K3
  cs->setUniform("phase",3.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // K4
  cs->setUniform("phase",4.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // Lopullinen tulos.
  cs->setUniform("phase",5.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void LumihiutaleSystem::draw(const glm::mat4& mvp)
{
  glm::mat4 mx = glm::mat4(1.0f);

  auto drawBuffer = VertexBufferManager::getInstance().getByKey(LumihiutaleSystem::INITIAL_BUFFER); 

  drawBuffer->bind();

  //auto render_shader_name = ProgramState::getInstance().getMetadata()->meshShader;
  Shader* shader = ShaderManager::getInstance().getByKey("lumi_render");
  shader->bind();
  shader->setUniform("MVP", mvp);
  shader->setUniform("input_color", glm::vec3(1.0f,1.0f,1.0f));

  glDrawArrays(GL_POINTS, 0, pParticleCount);
}

/************************************************************
 *
 *  RuohikkoSystem
 *
 ***********************************************************/

void RuohikkoSystem::init()
{
  Log::getDebug().log("RuohikkoSystem::init");

  auto pgen = ParticleGenerator();
  auto pCount = pgen.generateGrass("ruohikko",600,1,1,1);
  pParticleCount = pCount; 
  Log::getDebug().log("Grass particle count == %.",std::to_string(pCount));
}

void RuohikkoSystem::takeStep(const float h)
{
  auto cs = ShaderManager::getInstance().getByKey(RuohikkoSystem::CS_NAME);
  cs->bind();

  auto metadata = ProgramState::getInstance().getMetadata();
  Texture* texture = TextureManager::getInstance().getByKey(metadata->texture3Dname);
  texture->use(0);
  cs->setUniform("diffuse3DTexture",0);

  auto wind_strength = ProgramState::getInstance().getWindStrength();

  cs->setUniform("h",ProgramState::getInstance().getTimeStep());
  cs->setUniform("time",ProgramState::getInstance().get_h_sum());
  cs->setUniform("wind_strength", ProgramState::getInstance().getStopWind() ? 0.0f : wind_strength);

  auto initial_data = VertexBufferManager::getInstance().getByKey(RuohikkoSystem::INITIAL_BUFFER);
  auto static_data = VertexBufferManager::getInstance().getByKey(RuohikkoSystem::STATIC_DATA_BUFFER);
  auto k1 = VertexBufferManager::getInstance().getByKey(RuohikkoSystem::K1);
  auto k2 = VertexBufferManager::getInstance().getByKey(RuohikkoSystem::K2);
  auto k3 = VertexBufferManager::getInstance().getByKey(RuohikkoSystem::K3);
  auto k4 = VertexBufferManager::getInstance().getByKey(RuohikkoSystem::K4);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, initial_data->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, static_data->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, k1->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, k2->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, k3->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, k4->getHandle());

  const static int X = pParticleCount;
  const static int Y = 1;

  //Log::getDebug().log("Particle_count::%",std::to_string(pParticleCount));

  // K1
  cs->setUniform("phase",1.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // K2
  cs->setUniform("phase",2.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // K3
  cs->setUniform("phase",3.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // K4
  cs->setUniform("phase",4.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  // Lopullinen tulos.
  cs->setUniform("phase",5.0f);
  glDispatchCompute(X,Y,1);
  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void RuohikkoSystem::draw(const glm::mat4& mvp)
{
  glm::mat4 mx = glm::mat4(1.0f);

  auto drawBuffer = VertexBufferManager::getInstance().getByKey("piirto_indeksit"); 

  glLineWidth(1);
  drawBuffer->bind();

  Shader* shader = ShaderManager::getInstance().getByKey("jousi_particle_render");
  shader->bind();
  shader->setUniform("MVP", mvp);
  shader->setUniform("input_color", glm::vec3(0.0f,0.4f,0.0));

  auto initial_data = VertexBufferManager::getInstance().getByKey(RuohikkoSystem::INITIAL_BUFFER);
  auto static_data = VertexBufferManager::getInstance().getByKey(RuohikkoSystem::STATIC_DATA_BUFFER);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, initial_data->getHandle());
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, static_data->getHandle());

//  auto render_shader_name = ProgramState::getInstance().getMetadata()->meshShader;
//  Shader* shader = ShaderManager::getInstance().getByKey(render_shader_name);
//  shader->bind();
//  shader->setUniform("MVP", mvp);
//  shader->setUniform("input_color", glm::vec3(0.0f,0.4f,0.0));

  glDrawArrays(GL_POINTS, 0, pParticleCount-1);
  glLineWidth(3);
}
