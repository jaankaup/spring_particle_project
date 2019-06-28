#ifndef TIMER_H
#define TIMER_H
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <chrono>
#include "../Utils/log.h"

/** A simple shader class with basic shader operations. */
class Timer
{
	public:

		~Timer();
    static Timer& getInstance();


    void reset();
    uint64_t ticks() const;

	private:
    uint64_t pTimeNow = 0;

		Timer();
};


#endif // TIMER_H
