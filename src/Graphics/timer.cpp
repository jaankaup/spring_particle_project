#include "timer.h"

Timer& Timer::getInstance()
{
    static Timer instance;
    return instance;
}

Timer::Timer()
{
    pTimeNow = std::chrono::system_clock::now().time_since_epoch().count();
}

Timer::~Timer()
{

}

void Timer::reset()
{
    pTimeNow = std::chrono::system_clock::now().time_since_epoch().count();
}

uint64_t Timer::ticks() const
{
    return std::chrono::system_clock::now().time_since_epoch().count() - pTimeNow;

}
