#include "ParticleSystem.h"
#include "../Graphics/vertexbuffer.h"
#include "../Graphics/programstate.h"
#include "../Graphics/manager.h"
#include "../Utils/misc.h"
#include "../Utils/log.h"

const std::string VerhoSystem::INITIAL_BUFFER = "verho_initial";
const std::string VerhoSystem::STATIC_DATA_BUFFER = "verho_static";
const std::string VerhoSystem::K1 = "verho_k1";
const std::string VerhoSystem::K2 = "verho_k2";
const std::string VerhoSystem::K3 = "verho_k3";
const std::string VerhoSystem::K4 = "verho_k4";
const std::string VerhoSystem::CS_NAME = "verho_shader";

uint32_t ParticleSystem::get_particle_count() const
{
  return pParticleCount;
}


void ParticleSystem::init()
{
  Log::getDebug().log("ParticleSystem::init");
}

void VerhoSystem::init()
{
  Log::getDebug().log("VerhoSystem::init");
  //auto ps = ProgramState::getInstance(); 

  // Luodaan tarvittavat puskurit.

  // Piirrettava data.
  auto initial_data = VertexBufferManager::getInstance().create(VerhoSystem::INITIAL_BUFFER);
  initial_data->init();

  // Staattinen data, partikkelin kaveri-idt, lepopituudet ym.
  auto static_data = VertexBufferManager::getInstance().create(VerhoSystem::STATIC_DATA_BUFFER);
  static_data->init_plain_buffer();

  // k1-k4 valiaikaispuskurit RK4:sta varten.
  auto k1 = VertexBufferManager::getInstance().create(VerhoSystem::K1);
  k1->init_plain_buffer();

  auto k2 = VertexBufferManager::getInstance().create(VerhoSystem::K2);
  k2->init_plain_buffer();

  auto k3 = VertexBufferManager::getInstance().create(VerhoSystem::K3);
  k3->init_plain_buffer();

  auto k4 = VertexBufferManager::getInstance().create(VerhoSystem::K4);
  k4->init_plain_buffer();

  // Luodaan compute shader, jos sita ei ole entuudestaan luotu.
  auto compute_shader = ShaderManager::getInstance().getByKey(VerhoSystem::CS_NAME);
  if (compute_shader == nullptr) compute_shader = ShaderManager::getInstance().create(VerhoSystem::CS_NAME);
  std::vector<std::string> src = {"shaders/jouset.comp"};
  compute_shader->build(src,false);

  const int width = 100;
  const int height = 100;
  pParticleCount = width*height;
  float offset = 0.01f;

  glm::vec3 a = glm::vec3(0.0f);
  glm::vec3 b = glm::vec3(offset,offset,0.0f);
  glm::vec3 c = glm::vec3(offset,0.0f,0.0f);

  float dist = glm::distance(a,b);
  float dist_horizontal = glm::distance(a,c);

  // jouset.comp shaderissa:
  // PVdata == struct { vec4 pos; vec4 vel; }
  auto array = new GLfloat[8*pParticleCount];

  // jouset.comp shaderissa:
  //
  // struct StaticData
  // {
  //   vec4 some_data;
  //   vec4 friends;
  //   vec4 friends2;
  //   vec4 rest_forces;
  //   vec4 rest_forces2;
  // };
  auto static_data_array = new GLfloat[20*pParticleCount];

  // Luodaan data.
  for (int j=0 ; j<width ; j++) {
  for (int i=0 ; i<height ; i++) {
    int position = i + width * j;
    int pah = position*8; 
    int static_pah = position*20; 

    // Partikkelin positio.
    array[pah] = (i%width)*offset; 
    array[pah+1] = (-1.0f)*j*offset; 
    array[pah+2] = 0.0f; 
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
}

void VerhoSystem::takeStep(const float h)
{
  //Log::getDebug().log("VerhoSystem::takeStep(%)",std::to_string(h));
  //auto ps = ProgramState::getInstance(); 
  auto cs = ShaderManager::getInstance().getByKey(VerhoSystem::CS_NAME);
  cs->bind();

  auto metadata = ProgramState::getInstance().getMetadata();
  Texture* texture = TextureManager::getInstance().getByKey(metadata->texture3Dname);
  texture->use(0);
  cs->setUniform("diffuse3DTexture",0);

  cs->setUniform("h",ProgramState::getInstance().getTimeStep());
  cs->setUniform("time",ProgramState::getInstance().get_h_sum());

  auto initial_data = VertexBufferManager::getInstance().getByKey(VerhoSystem::INITIAL_BUFFER);
  auto static_data = VertexBufferManager::getInstance().getByKey(VerhoSystem::STATIC_DATA_BUFFER);
  auto k1 = VertexBufferManager::getInstance().getByKey(VerhoSystem::K1);
  auto k2 = VertexBufferManager::getInstance().getByKey(VerhoSystem::K2);
  auto k3 = VertexBufferManager::getInstance().getByKey(VerhoSystem::K3);
  auto k4 = VertexBufferManager::getInstance().getByKey(VerhoSystem::K4);

//  Log::getDebug().log("% : % : % : % : % : %", std::to_string(initial_data->getHandle()),
//                                           std::to_string(static_data->getHandle()),
//                                           std::to_string(k1->getHandle()),
//                                           std::to_string(k2->getHandle()),
//                                           std::to_string(k3->getHandle()),
//                                           std::to_string(k4->getHandle()));

  // TODO: poista temp shaderista.
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

void VerhoSystem::draw(const glm::vec3& eyePosition, const glm::mat4& viewMatrix, const glm::mat4& projection)
{
  //Log::getDebug().log("VerhoSystem::draw()");
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glm::mat4 mx = glm::mat4(1.0f);

  auto drawBuffer = VertexBufferManager::getInstance().getByKey(VerhoSystem::INITIAL_BUFFER); 

  //Log::getDebug().log("VerhoSystem::draw(): drawBuffer->bind()");
  drawBuffer->bind();
  auto drawData = drawBuffer->getBufferData();
  auto float_array = std::get<0>(drawData).get();
  using namespace std;
  static bool print = false;
  if (print) {

    //auto static_data_buffer = VertexBufferManager::getInstance().getByKey(VerhoSystem::STATIC_DATA_BUFFER);
    //auto staticData = static_data_buffer->getBufferData();
    //auto sa = std::get<0>(staticData).get();

    //for (int i=0 ; i< 200 /*std::get<1>(drawData)/8 */ ; i++) {
    //  int offset = i*20;
    //  Log::getDebug().log("%. somedata == %,%,%,%",to_string(i),to_string(sa[offset]),to_string(sa[offset+1]),to_string(sa[offset+2]),to_string(sa[offset+3]));
    //  Log::getDebug().log("%. friends1 == %,%,%,%",to_string(i),to_string(sa[offset+4]),to_string(sa[offset+5]),to_string(sa[offset+6]),to_string(sa[offset+7]));
    //  Log::getDebug().log("%. friends2 == %,%,%,%",to_string(i),to_string(sa[offset+8]),to_string(sa[offset+9]),to_string(sa[offset+10]),to_string(sa[offset+11]));
    //  Log::getDebug().log("%. rest1 == %,%,%,%",to_string(i),to_string(sa[offset+12]),to_string(sa[offset+13]),to_string(sa[offset+14]),to_string(sa[offset+15]));
    //  Log::getDebug().log("%. rest2 == %,%,%,%",to_string(i),to_string(sa[offset+16]),to_string(sa[offset+17]),to_string(sa[offset+18]),to_string(sa[offset+19]));
    //}



    Log::getDebug().log("drawData_size == %", to_string(std::get<1>(drawData)));
    for (int i=0 ; i< 200 /*std::get<1>(drawData)/8 */ ; i++) {
      int offset = i*8;
      Log::getDebug().log("%. pos == %,%,%,%",to_string(i),to_string(float_array[offset]),to_string(float_array[offset+1]),to_string(float_array[offset+2]),to_string(float_array[offset+3]));
      Log::getDebug().log("%. vel == %,%,%,%",to_string(i),to_string(float_array[offset+4]),to_string(float_array[offset+5]),to_string(float_array[offset+6]),to_string(float_array[offset+7]));
    }
    print = false;
  }

  //Log::getDebug().log("VerhoSystem::draw(): haetaan meshShader");
  auto render_shader_name = ProgramState::getInstance().getMetadata()->meshShader;
  Shader* shader = ShaderManager::getInstance().getByKey(render_shader_name);
  shader->bind();
  shader->setUniform("MVP", projection * viewMatrix * mx);

  glDrawArrays(GL_POINTS, 0, pParticleCount);
}

