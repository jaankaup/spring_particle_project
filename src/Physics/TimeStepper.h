#ifndef TIMESTEPPER_H
#define TIMESTEPPER_H

class ParticleSystem;

class TimeStepper
{
  public:
    virtual void takeStep(ParticleSystem* particleSystem,float stepSize)=0;
};

//IMPLEMENT YOUR TIMESTEPPERS

class ForwardEuler : public TimeStepper
{
  void takeStep(ParticleSystem* particleSystem, float stepSize);
};

class Trapzoidal : public TimeStepper
{
  void takeStep(ParticleSystem* particleSystem, float stepSize);
};

/////////////////////////

//Provided
class RK4 : public TimeStepper
{
  void takeStep(ParticleSystem* particleSystem, float stepSize);
};

#endif // TIMESTEPPER_H
