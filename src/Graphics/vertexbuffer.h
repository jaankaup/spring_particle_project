#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vertexAttributes.h"
#include "shader.h"


class Vertexbuffer
{
  template <typename T> friend class Manager;
  friend class VertexBufferManager;
//  friend class std::vector<std::tuple<const std::string, Vertexbuffer>>;

	public:

    virtual void init();
    void bind() const;
    virtual void addData(const void* data, unsigned int size, const std::vector<std::string>& types) const;
    void createExampleCube();
    int createExamplePoints(const int dimensionX, const int dimensionY, const int dimensionZ);
    GLuint getHandle() const;
    GLuint getVAO() const;
    //void init_transform_feedback(const GLuint id, GLuint count);
    // TODO: change.
    int getCount() const;
    void setCount(const int count);
    //void dispose() const;

		virtual ~Vertexbuffer();

  protected:
    GLuint pId = 0;
    GLuint pVAO = 0;
    GLenum pTarget;
    GLenum pUsage;
    std::vector<float> pData;
    int pDataCount = 0;
		Vertexbuffer();

	private:
};

class ParticleBuffer : public Vertexbuffer
{
  template <typename T> friend class Manager;
  friend class VertexBufferManager;

  public:

    void init() override;
    virtual void addData(const void* data, unsigned int size, const std::vector<std::string>& types) const override;
    void takeStep(float t);
		virtual ~ParticleBuffer();

  private:

		ParticleBuffer();
    GLuint pTexture = 0;
    int pMax_group_size_x = 0;
    int pMax_group_size_y = 0;
    int pMax_group_size_z = 0;
    //GLuint pTemp1 = 0;
    //GLuint pTemp2 = 0;
    //GLuint pTemp3 = 0;
    //GLuint pTemp4 = 0;

};

//class TransformBuffer : public Vertexbuffer
//{
//  virtual void init() override;
//};


#endif // VERTEXBUFFER_H
