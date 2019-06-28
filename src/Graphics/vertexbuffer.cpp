#include "vertexbuffer.h"
#include "../Utils/log.h"

Vertexbuffer::Vertexbuffer()
{
}

Vertexbuffer::~Vertexbuffer()
{
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

void Vertexbuffer::init_transform_feedback(const GLuint id, GLuint count)
{
    glGenVertexArrays(1, &pVAO);
    glGenBuffers(1,&pId);
    bind();
    glBindBuffer(GL_ARRAY_BUFFER, id);
    pId = id;
    pDataCount = count;
    VertexAttributes vas;
    std::vector<std::string> types = {"3f"};
    vas.create_interleaved_attributes(types, false);
    vas.registerAttributes();
    glBindVertexArray(0);
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

void Vertexbuffer::dispose() const
{
//  Log::getDebug().log("Deleting vertexvuffer: %", std::to_string(pId));
  if (pId != 0) glDeleteBuffers(1,&pId);
  glDeleteVertexArrays(1, &pVAO);
  glDeleteBuffers(1, &pId);
}

int Vertexbuffer::getCount() const
{
    return pDataCount;
}

