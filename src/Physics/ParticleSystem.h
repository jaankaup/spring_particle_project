#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include <vector>
#include <vecmath.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

class ParticleSystem
{
public:

  ParticleSystem(int numParticles=0);

  int m_numParticles;
  
  // for a given state, evaluate derivative f(X,t)
  virtual std::vector<glm::vec3> evalF(std::vector<glm::vec3> state) = 0;
  
  // getter method for the system's state
  std::vector<glm::vec3> getState(){ return m_vVecState; };
  
  // setter method for the system's state
  void setState(const std::vector<glm::vec3>  & newState) { m_vVecState = newState; };
  
  virtual void draw() = 0;
  
protected:

  std::vector<glm::vec3> m_vVecState;
  
};

#endif
