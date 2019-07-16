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
    void addData(const void* data, unsigned int size, const std::vector<std::string>& types) const override;
		virtual ~ParticleBuffer();
    void novoeha(const void* data, const void* static_data, unsigned int size, const std::vector<std::string>& types) const;
    void takeStep(float t);

  private:

		ParticleBuffer();
//    GLuint* pTexture = new GLuint(6);
    int pMax_group_size_x = 0;
    int pMax_group_size_y = 0;
    int pMax_group_size_z = 0;
    GLuint pTemp = 0;
    GLuint pStatic_data = 0;
    GLuint pK1 = 0;
    GLuint pK2 = 0;
    GLuint pK3 = 0;
    GLuint pK4 = 0;
};

//class TransformBuffer : public Vertexbuffer
//{
//  virtual void init() override;
//};


#endif // VERTEXBUFFER_H
