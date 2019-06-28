#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Command
{
  std::string name;
  GLuint vao;
  GLenum draw;
//  std::string textureName;
  std::string shaderName;
  uint32_t startIndex = 0;
  uint32_t count = 0;
  glm::mat4 modelMatrix;
//  int block_size;
//  int cube_count_x;
//  int cube_count_y;
//  int cube_count_z;
//  glm::vec3 start_point;
};

class Model
{

	public:
		Model();
		~Model();

    // Setters.
    void addCommand(const Command& command);
    void setCameraPosition(const glm::vec3& cPos);
    void setDisabled(const bool disabled);

    // Getters.
    std::vector<Command>* getCommands() const;
    glm::vec3 getCameraPosition() const;
    bool getDisabled() const;

	private:
    std::vector<Command> pCommands;

    // Initial staring point for camera. Optional.
    glm::vec3 pCameraStart;

    // Model is disabled.
    bool pDisabled = true;
//    std::vector<VertexBuffer> pVertexBuffers;
};

#endif // MODEL_H
