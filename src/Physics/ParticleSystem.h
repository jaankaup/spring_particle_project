#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
//#undef MANAGER_H
//#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/transform.hpp>
//

class ParticleSystem
{
  template <typename T> friend class Manager;
  friend class ParticleSystemManager;

  public:

    // Initialize particle system.
    virtual void init();

    // Take step.
    virtual void takeStep(const float h) {}; 

    // Draw particles.
    virtual void draw(const glm::mat4& mvp) {}; 

    virtual ~ParticleSystem() {};

    uint32_t get_particle_count() const;
    
  protected:
  
    uint32_t pParticleCount = 0;
    ParticleSystem() {};
    
  private:
};

class VerhoSystem : public ParticleSystem
{
  template <typename T> friend class Manager;
  friend class ParticleSystemManager;

  public:
    virtual void init() override;
    virtual void takeStep(const float h) override;
    virtual void draw(const glm::mat4& mvp) override;
    virtual ~VerhoSystem() {};

  private:
    static const std::string INITIAL_BUFFER;
    static const std::string STATIC_DATA_BUFFER;
    static const std::string K1;
    static const std::string K2;
    static const std::string K3;
    static const std::string K4;
    static const std::string CS_NAME;

};

#endif
