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

int Vertexbuffer::getCount() const
{
    return pDataCount;
}

void Vertexbuffer::setCount(const int count)
{
  pDataCount = count;
}

