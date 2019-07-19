#include <chrono>
#include <assert.h>
#include "vertexbuffer.h"
#include "../Utils/log.h"

#include "manager.h"

Vertexbuffer::Vertexbuffer()
{
}

Vertexbuffer::~Vertexbuffer()
{
  if (pId != 0) glDeleteBuffers(1,&pId);
  if (pVAO != 0) glDeleteVertexArrays(1, &pVAO);
  // glDeleteBuffers(1, &pId); // WHY
}


void Vertexbuffer::bind() const
{
  if (pVAO != 0) glBindVertexArray(pVAO);
}

void Vertexbuffer::init()
{
    glGenVertexArrays(1, &pVAO);
    glGenBuffers(1,&pId);
    pTarget = GL_ARRAY_BUFFER;
    pUsage = GL_DYNAMIC_DRAW;
}

void Vertexbuffer::init_plain_buffer()
{
    glGenBuffers(1,&pId);
    pTarget = GL_ARRAY_BUFFER;
    pUsage = GL_DYNAMIC_DRAW;
}

GLuint Vertexbuffer::getHandle() const
{
  return pId;
}

void Vertexbuffer::populate_data(const void* data, unsigned int size)
{
  Log::getDebug().log("Vertexbuffer::populate_data: size == %.",std::to_string(size));

  // Eka kerta kun puskuriin laitetaan tavaraa. Luodaan tilaa puskuriin sizen
  // verran.
  glBindBuffer(GL_ARRAY_BUFFER, pId);
  if (pData_size == 0) {
    if (data == nullptr) {
      Log::getDebug().log("Vertexbuffer::populate_data: creating empty data.");
      glBufferData(GL_ARRAY_BUFFER,size, NULL, pUsage);
    }
    else {
      Log::getDebug().log("Vertexbuffer::populate_data: populating data.");
      glBufferData(GL_ARRAY_BUFFER,size, data, pUsage);
    }
    pData_size = size;
    return;
  }

  // Jos myohemmin on tarve muokata dataa, niin taalla se kay.
  assert(pData_size < size);
  glBindBuffer(GL_ARRAY_BUFFER, pId);
  void *ptr = glMapNamedBufferRange(pId, 0, size, GL_WRITE_ONLY);
  memcpy(ptr, data, size);
  glUnmapNamedBuffer(pId);
}

void Vertexbuffer::addData(const void* data, unsigned int size, const std::vector<std::string>& types)
{
  Log::getDebug().log("Vertexbuffer::addData");

  assert(size == 0); 
  assert(pData_size > 0); 

  //Log::getDebug().log("Vertexbuffer::addData: %", std::to_string(size));
  bind();
  glBindBuffer(pTarget, pId);
  glBufferData(pTarget,size, data, pUsage);
  VertexAttributes vas;
  vas.create_interleaved_attributes(types, false);
  vas.registerAttributes();
  glBindVertexArray(0);
  pData_size = size;
}


GLuint Vertexbuffer::getVAO() const
{
  return pVAO;
}

std::tuple<std::unique_ptr<float[]>,uint32_t> Vertexbuffer::getBufferData() const
{
  int size_using_float = pData_size/sizeof(float);
  auto result = std::unique_ptr<GLfloat[]>(new GLfloat[size_using_float]);
  glGetNamedBufferSubData(pId, 0, pData_size, result.get());
  return std::make_tuple(std::move(result), size_using_float);
  //return std::move(result);
}

int Vertexbuffer::createExamplePoints(const int dimensionX, const int dimensionY, const int dimensionZ)
{
  int size = dimensionX * dimensionY * dimensionZ;
  for (int i=0 ; i<dimensionX ; ++i) {
  for (int j=0 ; j<dimensionY ; ++j) {
  for (int k=0 ; k<dimensionZ ; ++k) {
    pData.push_back((float)i);
    pData.push_back((float)j);
    pData.push_back((float)k);
  }}};

  std::vector<std::string> types = {"3f"};
  addData(&pData[0], pData.size() * sizeof(GL_FLOAT),types);
  int dataSize = pData.size();
  pDataCount = size;
  return size;
}

//void Vertexbuffer::dispose() const
//{
////  Log::getDebug().log("Deleting vertexvuffer: %", std::to_string(pId));
//  if (pId != 0) glDeleteBuffers(1,&pId);
//  glDeleteVertexArrays(1, &pVAO);
//  glDeleteBuffers(1, &pId);
//}

int Vertexbuffer::getCount() const
{
    return pDataCount;
}

void Vertexbuffer::setCount(const int count)
{
  pDataCount = count;
}

//// PARTICLE BUFFER
  
ParticleBuffer::ParticleBuffer()
{
//  for (int i=0 ; i<6 ; i++) {
//    pTexture[i] = 0;     
//  }
}

void ParticleBuffer::init()
{
    Log::getInfo().log("ParticleBuffer::init");
    glGenVertexArrays(1, &pVAO);
    glGenBuffers(1,&pId);
//    glGenBuffers(1,&pTemp1);
//    glBindBuffer(GL_SHADER_STORAGE_BUFFER, pTemp1);
////    glBufferData(pTarget, 8*sizeof(float), NULL, pTemp1);
//    glGenBuffers(1,&pTemp2);
//    glGenBuffers(1,&pTemp3);
//    glGenBuffers(1,&pTemp4);
    //pTarget = GL_SHADER_STORAGE_BUFFER;
    glGenBuffers(1,&pTemp);
    glGenBuffers(1,&pStatic_data);
    glGenBuffers(1,&pK1);
    glGenBuffers(1,&pK2);
    glGenBuffers(1,&pK3);
    glGenBuffers(1,&pK4);
    pTarget = GL_ARRAY_BUFFER;
    pUsage = GL_DYNAMIC_DRAW;

    int work_group_sizes[3];

    int result;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &result); 
    Log::getDebug().log("Maximum texture size = %.", std::to_string(result));

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_group_sizes[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_group_sizes[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_group_sizes[2]);

    pMax_group_size_x = work_group_sizes[0];
    pMax_group_size_y = work_group_sizes[1];
    pMax_group_size_z = work_group_sizes[2];

    Log::getDebug().log("Max_work_group_sizes: x = %, y = %, z = %", pMax_group_size_x,pMax_group_size_y,pMax_group_size_z);

    // Lets create n*4 texture for computeshader.
    // We bind it to the "binding=1"
////    auto particleCount = ProgramState::getInstance().getParticleCount();
////    Log::getDebug().log("t_w = %.", std::to_string(particleCount));
//    glBindImageTexture(1, pTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

ParticleBuffer::~ParticleBuffer()
{
  //if (pId != 0) glDeleteBuffers(1,&pId);
//  bool destroy_textures = false;
//  for (int i=0 ; i<6 ; i++) {
//    if (pTexture[i] != 0) destroy_textures = true;     
//  }
//  if (destroy_textures) glDeleteTextures(7,pTexture);
//  delete[] pTexture;
  if (pTemp != 0) glDeleteBuffers(1,&pTemp);
  if (pStatic_data != 0) glDeleteBuffers(1,&pStatic_data);
  if (pK1 != 0) glDeleteBuffers(1,&pK1);
  if (pK2 != 0) glDeleteBuffers(1,&pK2);
  if (pK3 != 0) glDeleteBuffers(1,&pK3);
  if (pK4 != 0) glDeleteBuffers(1,&pK4);
}

void ParticleBuffer::takeStep(float h)
{
  //float t = float(ProgramState::getInstance().getTimeDeltaMilliseconds())/500.0f;
//  float t_temp = h; 
//
    using namespace std;
    auto microseconds_now = chrono::high_resolution_clock::now().time_since_epoch();
    auto microseconds_now2 = chrono::duration_cast<chrono::microseconds>(microseconds_now).count();
    const static auto since = microseconds_now2; 
//    static auto sum;
//    sum += h;



    //static auto microseconds_before = std::high_resolution_clock::period::now();

//  Log::getDebug().log("t_temp = %",std::to_string(t_temp));

//  static int counter = 0;
//  counter++;
//  Log::getDebug().log("counter = %",std::to_string(counter));
//  glBindVertexArray(0);
  //bind();

  Shader* compute = ShaderManager::getInstance().getByKey("particle1");
  compute->bind();

  auto metadata = ProgramState::getInstance().getMetadata();
  Texture* texture = TextureManager::getInstance().getByKey(metadata->texture3Dname);
  texture->use(0);
  compute->setUniform("diffuse3DTexture",0);

  compute->setUniform("time",(microseconds_now2-since)*0.0000001f);
//  Log::getDebug().log("time = %.",std::to_string((microseconds_now2-since)*0.00000001f));
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pId);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, pTemp);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, pStatic_data);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, pK1);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, pK2);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, pK3);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, pK4);
    

//  /* Tekstruurin bindaus shaderiin "layout (rgba32f, binding = n) uniform image2D xxxx;" */
//  glBindImageTexture(0, pTexture[0], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
//  glBindImageTexture(1, pTexture[1], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
//  glBindImageTexture(2, pTexture[2], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
//  glBindImageTexture(3, pTexture[3], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
//  glBindImageTexture(4, pTexture[4], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
//  glBindImageTexture(5, pTexture[5], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
  
//  do {
    //glBindBuffer(GL_SHADER_STORAGE_BUFFER, pId);
    // Euler method. 

    // The time.
    compute->setUniform("h",h);
    compute->setUniform("time",h);
  
    const static int X = 100;
    const static int Y = 1;

//    compute->setUniform("phase",5.0f);
//    glDispatchCompute(X,1,1);
//    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
//
//    compute->setUniform("phase",6.0f);
//    glDispatchCompute(X,1,1);
//    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    
    compute->setUniform("phase",1.0f);
    glDispatchCompute(X,Y,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    compute->setUniform("phase",2.0f);
    glDispatchCompute(X,Y,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    compute->setUniform("phase",3.0f);
    glDispatchCompute(X,Y,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    compute->setUniform("phase",4.0f);
    glDispatchCompute(X,Y,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    compute->setUniform("phase",5.0f);
    glDispatchCompute(X,Y,1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

//    compute->setUniform("phase",6.0f);
//    glDispatchCompute(X,1,1);
//    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    auto particle_count = ProgramState::getInstance().getParticleCount();
    //bool printJuttu = counter > 2 ? false : true;
    bool printJuttu = false;
    if (printJuttu)
    {
      GLfloat* feedback = new GLfloat[particle_count*8];
      GLfloat* textureData = new GLfloat[particle_count*12];
      glGetNamedBufferSubData(pId, 0, sizeof(float)*8*particle_count, feedback);
      glGetNamedBufferSubData(pStatic_data, 0, sizeof(float)*12*particle_count, textureData);
////      //glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA,GL_FLOAT, pTexture[0]);
////
////
      for (int i=0 ; i<particle_count ; i++)
      {
        int j = i*8;
        int k = i*12;
////
        Log::getDebug().log("%.",std::to_string(i));
        Log::getDebug().log("%,%,%,%.",std::to_string(j),std::to_string(j+1),std::to_string(j+2),std::to_string(j+3));
        Log::getDebug().log("INPUT::pos (%,%,%,%)", std::to_string(feedback[j]),
                                                    std::to_string(feedback[j+1]),
                                                    std::to_string(feedback[j+2]),
                                                    std::to_string(feedback[j+3]));
        Log::getDebug().log("INPUT::vel (%,%,%,%)", std::to_string(feedback[j+4]),
                                                    std::to_string(feedback[j+5]),
                                                    std::to_string(feedback[j+6]),
                                                    std::to_string(feedback[j+7]));
        Log::getDebug().log("Static::other (%,%,%,%)", std::to_string(textureData[k]),
                                                       std::to_string(textureData[k+1]),
                                                       std::to_string(textureData[k+2]),
                                                       std::to_string(textureData[k+3]));
        Log::getDebug().log("Static::friends (%,%,%,%)", std::to_string(textureData[k+4]),
                                                         std::to_string(textureData[k+5]),
                                                         std::to_string(textureData[k+6]),
                                                         std::to_string(textureData[k+7]));
        Log::getDebug().log("Static::rest (%,%,%,%)", std::to_string(textureData[k+8]),
                                                      std::to_string(textureData[k+9]),
                                                      std::to_string(textureData[k+10]),
                                                      std::to_string(textureData[k+11]));
      }
      delete[] feedback; 
      delete[] textureData; 
    }
////        Log::getDebug().log("INPUT::vel (%,%,%,%)", std::to_string(feedback[j+4]),
////                                                    std::to_string(feedback[j+5]),
////                                                    std::to_string(feedback[j+6]),
////                                                    std::to_string(feedback[j+7]));
////        Log::getDebug().log("INPUT::friends (%,%,%,%)", std::to_string(feedback[j+8]),
////                                                        std::to_string(feedback[j+9]),
////                                                        std::to_string(feedback[j+10]),
////                                                        std::to_string(feedback[j+11]));
////        Log::getDebug().log("INPUT::rest_forces (%,%,%,%)", std::to_string(feedback[j+12]),
////                                                            std::to_string(feedback[j+13]),
////                                                            std::to_string(feedback[j+14]),
////                                                            std::to_string(feedback[j+15]));
////        Log::getDebug().log("K1::pos (%,%,%,%)", std::to_string(textureData[k]),
////                                                 std::to_string(textureData[k+1]),
////                                                 std::to_string(textureData[k+2]),
////                                                 std::to_string(textureData[k+3]));
////        Log::getDebug().log("K1::vel (%,%,%,%)", std::to_string(textureData[k+4]),
////                                                 std::to_string(textureData[k+5]),
////                                                 std::to_string(textureData[k+6]),
////                                                 std::to_string(textureData[k+7]));
////        Log::getDebug().log("K1::friends (%,%,%,%)", std::to_string(textureData[k+8]),
////                                                     std::to_string(textureData[k+9]),
////                                                     std::to_string(textureData[k+10]),
////                                                     std::to_string(textureData[k+11]));
////        Log::getDebug().log("K1::rest_forces (%,%,%,%)", std::to_string(textureData[k+12]),
////                                                         std::to_string(textureData[k+13]),
////                                                         std::to_string(textureData[k+14]),
////                                                         std::to_string(textureData[k+15]));
////        Log::getDebug().log("K2::pos (%,%,%,%)", std::to_string(textureData[k+16]),
////                                                 std::to_string(textureData[k+17]),
////                                                 std::to_string(textureData[k+18]),
////                                                 std::to_string(textureData[k+19]));
////        Log::getDebug().log("K2::vel (%,%,%,%)", std::to_string(textureData[k+20]),
////                                                 std::to_string(textureData[k+21]),
////                                                 std::to_string(textureData[k+22]),
////                                                 std::to_string(textureData[k+23]));
////        Log::getDebug().log("K2::friends (%,%,%,%)", std::to_string(textureData[k+24]),
////                                                     std::to_string(textureData[k+25]),
////                                                     std::to_string(textureData[k+26]),
////                                                     std::to_string(textureData[k+27]));
////        Log::getDebug().log("K2::rest_forces (%,%,%,%)", std::to_string(textureData[k+28]),
////                                                         std::to_string(textureData[k+29]),
////                                                         std::to_string(textureData[k+30]),
////                                                         std::to_string(textureData[k+31]));
////        Log::getDebug().log("K3::pos (%,%,%,%)", std::to_string(textureData[k+32]),
////                                                 std::to_string(textureData[k+33]),
////                                                 std::to_string(textureData[k+34]),
////                                                 std::to_string(textureData[k+35]));
////        Log::getDebug().log("K3::vel (%,%,%,%)", std::to_string(textureData[k+36]),
////                                                 std::to_string(textureData[k+37]),
////                                                 std::to_string(textureData[k+38]),
////                                                 std::to_string(textureData[k+39]));
////        Log::getDebug().log("K3::friends (%,%,%,%)", std::to_string(textureData[k+40]),
////                                                     std::to_string(textureData[k+41]),
////                                                     std::to_string(textureData[k+42]),
////                                                     std::to_string(textureData[k+43]));
////        Log::getDebug().log("K3::rest_forces (%,%,%,%)", std::to_string(textureData[k+44]),
////                                                         std::to_string(textureData[k+45]),
////                                                         std::to_string(textureData[k+46]),
////                                                         std::to_string(textureData[k+47]));
////        Log::getDebug().log("K4::pos (%,%,%,%)", std::to_string(textureData[k+48]),
////                                                 std::to_string(textureData[k+49]),
////                                                 std::to_string(textureData[k+50]),
////                                                 std::to_string(textureData[k+51]));
////        Log::getDebug().log("K4::vel (%,%,%,%)", std::to_string(textureData[k+52]),
////                                                 std::to_string(textureData[k+53]),
////                                                 std::to_string(textureData[k+54]),
////                                                 std::to_string(textureData[k+55]));
////        Log::getDebug().log("K4::friends (%,%,%,%)", std::to_string(textureData[k+56]),
////                                                     std::to_string(textureData[k+57]),
////                                                     std::to_string(textureData[k+58]),
////                                                     std::to_string(textureData[k+59]));
////        Log::getDebug().log("K4::rest_forces (%,%,%,%)", std::to_string(textureData[k+60]),
////                                                         std::to_string(textureData[k+61]),
////                                                         std::to_string(textureData[k+62]),
////                                                         std::to_string(textureData[k+63]));
////        counter++;
//////        if (counter == 1) break;
////      }
////
////       delete[] feedback; 
////       delete[] textureData;
////    }
//    t_temp = t_temp + h;
//  } while (t_temp < t);
//    do {
//      Log::getDebug().log("t = %",std::to_string(t));
//      Log::getDebug().log("t_temp = %",std::to_string(t_temp));
//      t_temp = t_temp + h;
//    } while (t_temp < t);

//    }
}

void ParticleBuffer::novoeha(const void* data, const void* static_data, unsigned int size, const std::vector<std::string>& types) const
{
  Log::getInfo().log("ParticleBuffer::addData2");
  //auto particle_count = ProgramState::getInstance().getParticleCount();
  bind();

  // Luodaan piirto-purkuri (initial_data).
  glBindBuffer(pTarget, pId);
  glBufferData(pTarget,size*8, data, pUsage);

  // Maaritellaan kuinka data piirretaan.
  VertexAttributes vas;
  vas.create_interleaved_attributes(types, false);
  vas.registerAttributes();
  glBindVertexArray(0);

////  // Luodaan staattinen data.
////  glBindBuffer(GL_SHADER_STORAGE_BUFFER, pStatic_data);
////  glBufferData(GL_SHADER_STORAGE_BUFFER,size*12, static_data, GL_STATIC_READ);
////
////  // Luodaan temp puskuri.
////  glBindBuffer(GL_SHADER_STORAGE_BUFFER, pTemp);
////  glBufferData(GL_SHADER_STORAGE_BUFFER,size*8, NULL, pUsage);
////
////  // Luodaan k1 puskuri.
////  glBindBuffer(GL_SHADER_STORAGE_BUFFER, pK1);
////  glBufferData(GL_SHADER_STORAGE_BUFFER,size*4, NULL, pUsage);
////
////  // Luodaan k2 puskuri.
////  glBindBuffer(GL_SHADER_STORAGE_BUFFER, pK2);
////  glBufferData(GL_SHADER_STORAGE_BUFFER,size*4, NULL, pUsage);
////
////  // Luodaan k3 puskuri.
////  glBindBuffer(GL_SHADER_STORAGE_BUFFER, pK3);
////  glBufferData(GL_SHADER_STORAGE_BUFFER,size*4, NULL, pUsage);
////
////  // Luodaan k4 puskuri.
////  glBindBuffer(GL_SHADER_STORAGE_BUFFER, pK4);
////  glBufferData(GL_SHADER_STORAGE_BUFFER,size*4, NULL, pUsage);

  glBindBuffer(GL_ARRAY_BUFFER, pStatic_data);
  glBufferData(GL_ARRAY_BUFFER,size*20, static_data, GL_STATIC_READ);

  // Luodaan temp puskuri.
  glBindBuffer(GL_ARRAY_BUFFER, pTemp);
  glBufferData(GL_ARRAY_BUFFER,size*8, NULL, pUsage);

  // Luodaan k1 puskuri.
  glBindBuffer(GL_ARRAY_BUFFER, pK1);
  glBufferData(GL_ARRAY_BUFFER,size*4, NULL, pUsage);

  // Luodaan k2 puskuri.
  glBindBuffer(GL_ARRAY_BUFFER, pK2);
  glBufferData(GL_ARRAY_BUFFER,size*4, NULL, pUsage);

  // Luodaan k3 puskuri.
  glBindBuffer(GL_ARRAY_BUFFER, pK3);
  glBufferData(GL_ARRAY_BUFFER,size*4, NULL, pUsage);

  // Luodaan k4 puskuri.
  glBindBuffer(GL_ARRAY_BUFFER, pK4);
  glBufferData(GL_ARRAY_BUFFER,size*4, NULL, pUsage);
}

inline void ParticleBuffer::addData(const void* data, unsigned int size, const std::vector<std::string>& types) 
{
  //Log::getDebug().log("Vertexbuffer::addData: %", std::to_string(size));
  Log::getInfo().log("ParticleBuffer::addData");
  auto particle_count = ProgramState::getInstance().getParticleCount();
//  auto array = new GLfloat[4*4*particle_count];
//  for (int i=0 ; i<4*4*particle_count ; i++) {
//    array[i] = -77.0f;
//  }
  bind();
  glBindBuffer(pTarget, pId);
  //glBufferData(pTarget,size, data, pUsage);
  glBufferData(pTarget,size, NULL, pUsage);
  VertexAttributes vas;
  vas.create_interleaved_attributes(types, false);
  vas.registerAttributes();
  glBindVertexArray(0);

//  int t_w = ProgramState::getInstance().getParticlesWidth();
//  int t_h = ProgramState::getInstance().getParticlesHeight();
//  glGenTextures(6,pTexture);
//  for (int i=0; i<6 ; i++) {
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, pTexture[i]);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    i != 0 ? glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, t_w, t_h, 0, GL_RGBA, GL_FLOAT, NULL) 
//           : glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, t_w, t_h, 0, GL_RGBA, GL_FLOAT, data); 
//  }
}

