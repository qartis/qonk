// Copyright 2005 by Anthony Liekens anthony@liekens.net

#ifndef TIMER_H
#define TIMER_H

#include "SDL.h"
#include <iostream>
#include <string>
#include <sstream>
using namespace std;

class Timer {
private:
  static Uint32 startTime, pauseTime;
  static bool paused;
public:
  Timer();
  
  void start();
  void pause(); // this function does both pause and resume the clock
  
  bool isPaused();
  Uint32 getTime();
  string getTimeMMSSHH();
  string getTimeMMSS();

};

#endif
