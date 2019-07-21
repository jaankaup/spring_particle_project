#ifndef PARTICLEGENERATOR_H
#define PARTICLEGENERATOR_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

/* A class for genetaring random particle systems. */
struct ParticleLevel { glm::vec3 pos; int level; int particle_index; };

struct GrassParticle
{
  int index = 0;
  glm::vec4 pos;
  glm::vec4 vel;
  glm::vec4 other = glm::vec4(-1.0);
  glm::vec4 friends1 = glm::vec4(-1.0f);
  glm::vec4 friends2 = glm::vec4(-1.0f);
  glm::vec4 friends3 = glm::vec4(-1.0f);
  glm::vec4 rest_lengths = glm::vec4(-1.0f);
  glm::vec4 rest_lengths2 = glm::vec4(-1.0f);
  glm::vec4 rest_lengths3 = glm::vec4(-1.0f);
};

class ParticleGenerator
{

  public:
    int generateGrass(const std::string& name_prefix, const uint32_t grassCount, const uint32_t grassHeight, const uint32_t areaWidth, const uint32_t areaheight);
    
  private:

    float pRadius = 0.01f;
    std::vector<GrassParticle> pParticles;
};


#endif
