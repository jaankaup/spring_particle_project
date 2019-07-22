#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <memory>
#include <tuple>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vertexAttributes.h"
#include "shader.h"


// TODO: erillinen piirto-purkuri ja tavallinen data-puskuri periyttamalla.
class Vertexbuffer
{
  template <typename T> friend class Manager;
  friend class VertexBufferManager;
//  friend class std::vector<std::tuple<const std::string, Vertexbuffer>>;

	public:

    virtual void init(const GLenum target,const GLenum usage);
    void init_plain_buffer(const GLenum target,const GLenum usage);
    void bind() const;

    // Varaa bufferille kokoa @parametrin size verran ensimmaisella kerralla
    // kutsuttuna. Taman jalkeen jos yritetaan laitttaa dataa enemmman kuin 
    // ensimmaisella kerralla annettu size, niin heittaa poikkeuksen.
    void populate_data(const void* data, unsigned int size);

    // Asettaa datan piirrettavalle puskurille. taman jalkeen taytyy data
    // lisata populate_data funktiolla tai heittaa poikkeuksen. Maarittaa 
    // myos attribute-locatiot @types parametrin avulla. Esim. jos 
    // types = {"3f","2f","3f"}, niin tulkitsee datan interleavedina 
    // ja shaderissa taytyy olla
    // layout(location = 0) in vec3 joku_nimi0;
    // layout(location = 1) in vec2 joku_nimi1;
    // layout(location = 2) in vec3 joku_nimi2;
    virtual void addData(const void* data, unsigned int size, const std::vector<std::string>& types);

    // Luo marching cubesille dimensioiden mukaan piste-datan.
    int createExamplePoints(const int dimensionX, const int dimensionY, const int dimensionZ);

    // Palauttaa bufferin handlen.
    GLuint getHandle() const;

    // Palauttaa vao:n handlen.
    GLuint getVAO() const;

    // Palauttaa piirrettavien elementtien lukumaaran.
    int getCount() const;

    // Asettaa piirrettavien elementtien lukumaara.
    void setCount(const int count);

    // Palauttaa opengl puskurin datan. Tuplen ekassa elementissa on unique_ptr
    // float taulukkoon. Toisessa elementissa on floatien lukumaara.
    std::tuple<std::unique_ptr<float[]>,uint32_t> getBufferData() const;

		virtual ~Vertexbuffer();

  protected:
    GLuint pId = 0;
    GLuint pVAO = 0;
    GLenum pTarget;
    GLenum pUsage;
    std::vector<float> pData;
    int pDataCount = 0;
		Vertexbuffer();
    unsigned int pData_size = 0;

	private:
};

#endif // VERTEXBUFFER_H
