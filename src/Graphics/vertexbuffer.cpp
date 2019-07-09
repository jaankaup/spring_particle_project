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
  glBindVertexArray(pVAO);
}

void Vertexbuffer::init()
{
    glGenVertexArrays(1, &pVAO);
    glGenBuffers(1,&pId);
//    Log::getDebug().log("Created a vertex buffer: %", std::to_string(pId));
    pTarget = GL_ARRAY_BUFFER;
    pUsage = GL_STATIC_DRAW;
}

GLuint Vertexbuffer::getHandle() const
{
  return pId;
}

void Vertexbuffer::addData(const void* data, unsigned int size, const std::vector<std::string>& types) const
{
  //Log::getDebug().log("Vertexbuffer::addData: %", std::to_string(size));
  bind();
  glBindBuffer(pTarget, pId);
  glBufferData(pTarget,size, data, pUsage);
  VertexAttributes vas;
  vas.create_interleaved_attributes(types, false);
  vas.registerAttributes();
  glBindVertexArray(0);
}

void Vertexbuffer::createExampleCube()
{

  std::vector<glm::vec3> vData = {glm::vec3(1.0f , -1.0f, -1.0f),
                                  glm::vec3(1.0f , -1.0f, 1.0f),
                                  glm::vec3(-1.0f, -1.0f, 1.0f),
                                  glm::vec3(-1.0f, -1.0f, -1.0f),
                                  glm::vec3(1.0f , 1.0f, -1.0f),
                                  glm::vec3(1.0f, 1.0f, 1.0f),
                                  glm::vec3(-1.0f, 1.0f, 1.0f),
                                  glm::vec3(-1.0f, 1.0f, -1.0f)};

 std::vector<glm::vec2> tData = {glm::vec2(0.748573,0.750412),
                                 glm::vec2(0.749279,0.501284),
                                 glm::vec2(0.999110,0.501077),
                                 glm::vec2(0.999455,0.750380),
                                 glm::vec2(0.250471,0.500702),
                                 glm::vec2(0.249682,0.749677),
                                 glm::vec2(0.001085,0.750380),
                                 glm::vec2(0.001517,0.499994),
                                 glm::vec2(0.499422,0.500239),
                                 glm::vec2(0.500149,0.750166),
                                 glm::vec2(0.748355,0.998230),
                                 glm::vec2(0.500193,0.998728),
                                 glm::vec2(0.498993,0.250415),
                                 glm::vec2(0.748953,0.250920)};

  std::vector<glm::vec3> nData = {glm::vec3(0.0f , 0.0f , -1.0f),
                                  glm::vec3(-1.0f, -0.0f, 0.0f),
                                  glm::vec3(0.0f, -0.0f, 1.0f),
                                  glm::vec3(0.0f, 0.0f , 1.0f),
                                  glm::vec3(1.0f , -0.0f, 0.0f),
                                  glm::vec3(1.0f , 0.0f , 0.0f),
                                  glm::vec3(0.0f , 1.0f , 0.0f),
                                  glm::vec3(0.0f, -1.0f, 0.0f)};
  // Face 1
//  f 5/1/1 1/2/1 4/3/1 
  std::vector<glm::vec3> vs;
  std::vector<glm::vec2> ts;
  std::vector<glm::vec3> vn;
  vs.push_back(vData[4]);
  ts.push_back(tData[0]);
  vn.push_back(nData[0]);
  vs.push_back(vData[0]);
  ts.push_back(tData[1]);
  vn.push_back(nData[0]);
  vs.push_back(vData[3]);
  ts.push_back(tData[2]);
  vn.push_back(nData[0]);

  // Face2
//  f 5/1/1 4/3/1 8/4/1
  vs.push_back(vData[4]);
  ts.push_back(tData[0]);
  vn.push_back(nData[0]);
  vs.push_back(vData[3]);
  ts.push_back(tData[2]);
  vn.push_back(nData[0]);
  vs.push_back(vData[7]);
  ts.push_back(tData[3]);
  vn.push_back(nData[0]);

  // Face3
//  f 3/5/2 7/6/2 8/7/2
  vs.push_back(vData[2]);
  ts.push_back(tData[4]);
  vn.push_back(nData[1]);
  vs.push_back(vData[6]);
  ts.push_back(tData[5]);
  vn.push_back(nData[1]);
  vs.push_back(vData[7]);
  ts.push_back(tData[6]);
  vn.push_back(nData[1]);

  // Face4
//  f 3/5/2 8/7/2 4/8/2
  vs.push_back(vData[2]);
  ts.push_back(tData[4]);
  vn.push_back(nData[1]);
  vs.push_back(vData[7]);
  ts.push_back(tData[6]);
  vn.push_back(nData[1]);
  vs.push_back(vData[3]);
  ts.push_back(tData[7]);
  vn.push_back(nData[1]);

  // Face5
//  f 2/9/3 6/10/3 3/5/3
  vs.push_back(vData[1]);
  ts.push_back(tData[8]);
  vn.push_back(nData[2]);
  vs.push_back(vData[5]);
  ts.push_back(tData[9]);
  vn.push_back(nData[2]);
  vs.push_back(vData[2]);
  ts.push_back(tData[4]);
  vn.push_back(nData[2]);

  // Face6
//  f 6/10/4 7/6/4 3/5/4
  vs.push_back(vData[5]);
  ts.push_back(tData[9]);
  vn.push_back(nData[3]);
  vs.push_back(vData[6]);
  ts.push_back(tData[5]);
  vn.push_back(nData[3]);
  vs.push_back(vData[2]);
  ts.push_back(tData[4]);
  vn.push_back(nData[3]);

  // Face7
//  f 1/2/5 5/1/5 2/9/5
  vs.push_back(vData[0]);
  ts.push_back(tData[1]);
  vn.push_back(nData[4]);
  vs.push_back(vData[4]);
  ts.push_back(tData[0]);
  vn.push_back(nData[4]);
  vs.push_back(vData[1]);
  ts.push_back(tData[8]);
  vn.push_back(nData[4]);

  // Face8
//  f 5/1/6 6/10/6 2/9/6
  vs.push_back(vData[4]);
  ts.push_back(tData[0]);
  vn.push_back(nData[5]);
  vs.push_back(vData[5]);
  ts.push_back(tData[9]);
  vn.push_back(nData[5]);
  vs.push_back(vData[1]);
  ts.push_back(tData[8]);
  vn.push_back(nData[5]);

  // Face9
//  f 5/1/7 8/11/7 6/10/7
  vs.push_back(vData[4]);
  ts.push_back(tData[0]);
  vn.push_back(nData[6]);
  vs.push_back(vData[7]);
  ts.push_back(tData[10]);
  vn.push_back(nData[6]);
  vs.push_back(vData[5]);
  ts.push_back(tData[9]);
  vn.push_back(nData[6]);

  // Face10
 // f 8/11/7 7/12/7 6/10/7
  vs.push_back(vData[7]);
  ts.push_back(tData[10]);
  vn.push_back(nData[6]);
  vs.push_back(vData[6]);
  ts.push_back(tData[11]);
  vn.push_back(nData[6]);
  vs.push_back(vData[5]);
  ts.push_back(tData[9]);
  vn.push_back(nData[6]);

  // Face11
//  f 1/2/8 2/9/8 3/13/8
  vs.push_back(vData[0]);
  ts.push_back(tData[1]);
  vn.push_back(nData[7]);
  vs.push_back(vData[1]);
  ts.push_back(tData[8]);
  vn.push_back(nData[7]);
  vs.push_back(vData[2]);
  ts.push_back(tData[12]);
  vn.push_back(nData[7]);

  // Face12
//  f 1/2/8 3/13/8 4/14/8
  vs.push_back(vData[0]);
  ts.push_back(tData[1]);
  vn.push_back(nData[7]);
  vs.push_back(vData[2]);
  ts.push_back(tData[12]);
  vn.push_back(nData[7]);
  vs.push_back(vData[3]);
  ts.push_back(tData[13]);
  vn.push_back(nData[7]);
//
  for (unsigned int i=0 ; i<vs.size() ; i++)
  {
    pData.push_back(vs[i][0]); 
    pData.push_back(vs[i][1]); 
    pData.push_back(vs[i][2]); 
    pData.push_back(ts[i][0]); 
    pData.push_back(ts[i][1]); 
    pData.push_back(vn[i][0]); 
    pData.push_back(vn[i][1]); 
    pData.push_back(vn[i][2]); 
  }
  std::vector<std::string> types = {"3f","2f","3f"};
  addData(&pData[0], pData.size() * sizeof(GL_FLOAT),types);

  pDataCount = pData.size();
  pData.clear();
}

GLuint Vertexbuffer::getVAO() const
{
  return pVAO;
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
}

void ParticleBuffer::init()
{
    glGenVertexArrays(1, &pVAO);
    glGenBuffers(1,&pId);
//    glGenBuffers(1,&pTemp1);
////    glBindBuffer(pTarget, pTemp1);
////    glBufferData(pTarget, 8*sizeof(float), NULL, pTemp1);
//    glGenBuffers(1,&pTemp2);
//    glGenBuffers(1,&pTemp3);
//    glGenBuffers(1,&pTemp4);
    //pTarget = GL_SHADER_STORAGE_BUFFER;
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
    auto particleCount = ProgramState::getInstance().getParticleCount();
    Log::getDebug().log("t_w = %.", std::to_string(particleCount));
    int t_w = particleCount*2;
    int t_h = 2;
    glGenTextures(1,&pTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, t_w, t_h, 0, GL_RGBA, GL_FLOAT, NULL);
//    glBindImageTexture(1, pTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
}

ParticleBuffer::~ParticleBuffer()
{
  if (pTexture != 0) glDeleteTextures(1,&pTexture);
  //if (pTemp1 != 0) glDeleteBuffers(1,&pTemp1);
  //if (pTemp2 != 0) glDeleteBuffers(1,&pTemp2);
  //if (pTemp3 != 0) glDeleteBuffers(1,&pTemp3);
  //if (pTemp4 != 0) glDeleteBuffers(1,&pTemp4);
}

void ParticleBuffer::takeStep(float t)
{
  static int counter = 0;
  glBindVertexArray(0);
  //bind();

  Shader* compute = ShaderManager::getInstance().getByKey("particle1");
  compute->bind();

  // Lets bind the ssbo to binding = 0.
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, pId);
  // Lets bind the texture to binding = 1.
  glBindImageTexture(1, pTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

  //glBindBuffer(GL_SHADER_STORAGE_BUFFER, pId);
  // Euler method. 
  compute->setUniform("phase",1.0f);

  // The time.
  compute->setUniform("h",t);

  //Log::getDebug().log("Eka vaihe....");
  glDispatchCompute(20,1,1);
  //Log::getDebug().log("Lopetellaan dispatsaillu....");

  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  //Log::getDebug().log("Toka vaihe....");
  compute->setUniform("phase",2.0f);
  glDispatchCompute(20,1,1);

  glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

  auto particle_count = ProgramState::getInstance().getParticleCount();
  bool printJuttu = false;
  if (printJuttu)
  {
  GLfloat* feedback = new GLfloat[particle_count*8];
  GLfloat* textureData = new GLfloat[particle_count*16];
  glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float)*8*particle_count, feedback);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA,GL_FLOAT, textureData);


  for (int i=0 ; i<particle_count ; i++)
  {
    int j = i*8;

    Log::getDebug().log("%.",std::to_string(i));
    Log::getDebug().log("Pos        (%,%,%,%)", std::to_string(feedback[j]),
                                        std::to_string(feedback[j+1]),
                                        std::to_string(feedback[j+2]),
                                        std::to_string(feedback[j+3]));
    Log::getDebug().log("Vel        (%,%,%,%)", std::to_string(feedback[j+4]),
                                        std::to_string(feedback[j+5]),
                                        std::to_string(feedback[j+6]),
                                        std::to_string(feedback[j+7]));
    Log::getDebug().log("Texture Pos(%,%,%,%)", std::to_string(textureData[j]),
                                                std::to_string(textureData[j+1]),
                                                std::to_string(textureData[j+2]),
                                                std::to_string(textureData[j+3]));
    Log::getDebug().log("Texture Vel(%,%,%,%)", std::to_string(textureData[j+4]),
                                                std::to_string(textureData[j+5]),
                                                std::to_string(textureData[j+6]),
                                                std::to_string(textureData[j+7]));
    counter++;
  }

   delete[] feedback; 
   delete[] textureData;
  }
}

inline void ParticleBuffer::addData(const void* data, unsigned int size, const std::vector<std::string>& types) const
{
  //Log::getDebug().log("Vertexbuffer::addData: %", std::to_string(size));
  bind();
  glBindBuffer(pTarget, pId);
  glBufferData(pTarget,size, data, pUsage);
  VertexAttributes vas;
  vas.create_interleaved_attributes(types, false);
  vas.registerAttributes();
  //glBindBuffer(pTarget, pTemp1);
  //glBufferData(pTarget, size, NULL, pTemp1);
  glBindVertexArray(0);
}
