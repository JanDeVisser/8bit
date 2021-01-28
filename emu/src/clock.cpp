#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <thread>

#include "clock.h"

/**
 * @return Duration of a clock tick, in nanoseconds.
 */
unsigned long Clock::tick() const {
  return (unsigned long) round(1000000.0 / (2.0 * khz));
}

void Clock::sleep() const {
  auto start = std::chrono::high_resolution_clock::now();
  struct timespec req = {0};
  req.tv_sec = 0;
  req.tv_nsec = tick();
  nanosleep(&req, (struct timespec *) nullptr);
  auto finish = std::chrono::high_resolution_clock::now();
//  std::cout << std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count() << std::endl;
}

SystemError Clock::start() {
  cycle = Low;
  SystemError err = NoError;
  for (state = Running; err == NoError && state == Running; ) {
    sleep();
    if ((err = owner -> onRisingClockEdge()) != NoError) {
      break;
    };
    if ((err = owner -> onHighClock()) != NoError) {
      break;
    };
    sleep();
    if ((err = owner -> onFallingClockEdge()) != NoError) {
      break;
    };
    err = owner -> onLowClock();
  }
  return err;
}

void Clock::stop() {
  state = Stopped;
}