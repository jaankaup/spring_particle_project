#ifndef VERTEXATTRIBUES_H
#define VERTEXATTRIBUES_H

#include <vector>
#include <array>
#include <GL/glew.h>

struct VertexAttribute
{
  GLuint attributeLocation;
  GLint size;
  GLenum type;
  GLboolean normalized;
  GLsizei stride;
  int offset;
  //const GLvoid* offset;
};

/** A class for gl vertexattributes. */
class VertexAttributes
{
	public:
		VertexAttributes();
		~VertexAttributes();

    void addAttribute(const VertexAttribute& vAttribute);

    // Creates the vertex attribute array values. Function returns false on
    // errors. Otherwise true is returned. 
    // Example usage of @types. "3f","2f","3f" if data is type of
    // [vvvttnnn]+ 
    bool create_interleaved_attributes(const std::vector<std::string> types, bool normalized); 

    // Tell gl how to intrerpred the currently binded buffer.
    void registerAttributes() const;

	private:
    std::vector<VertexAttribute> pAttributes;
};


#endif // VERTEXATTRIBUES_H
