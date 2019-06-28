#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vertexAttributes.h"


class Vertexbuffer
{
  friend class VertexBufferManager;
//  friend class std::vector<std::tuple<const std::string, Vertexbuffer>>;

	public:
		Vertexbuffer();
		~Vertexbuffer();

    void init();
    void bind() const;
    void addData(const void* data, unsigned int size, const std::vector<std::string>& types) const;
    void createExampleCube();
    int createExamplePoints(const int dimensionX, const int dimensionY, const int dimensionZ);
    GLuint getHandle() const;
    GLuint getVAO() const;
    void init_transform_feedback(const GLuint id, GLuint count);
    // TODO: change.
    int getCount() const;
    void dispose() const;

	private:

    GLuint pId = 0;
    GLuint pVAO = 0;
    GLenum pTarget;
    GLenum pUsage;
    std::vector<float> pData;

    // TODO: change this. 
    int pDataCount = 0;

};


#endif // VERTEXBUFFER_H
