// Copyright 2005 by Anthony Liekens anthony@liekens.net

#include "timer.h"

Uint32 Timer::startTime;
Uint32 Timer::pauseTime;
bool Timer::paused;

Timer::Timer() {
  start();
}

void
Timer::start() {
  startTime = SDL_GetTicks();
  paused = false;
}

void
Timer::pause() {
  if( !paused ) {
    pauseTime = SDL_GetTicks();
    paused = true;
  } else {
    Uint32 pausedTime = SDL_GetTicks() - pauseTime;
    startTime += pausedTime;
    paused = false;
  }
}

Uint32
Timer::getTime() {
  if( !paused )
    return SDL_GetTicks() - startTime;
  else
    return pauseTime - startTime;
}

string 
Timer::getTimeMMSSHH() {
  if( paused )
    return "PA:US:ED";
  stringstream s;
  int hundreds = ( getTime() % 1000 ) / 10;
  int seconds = getTime() / 1000;
  int minutes = seconds / 60;
  seconds = seconds % 60;
  if( minutes < 10 )
    s << "0";
  s << minutes << ":";
  if( seconds < 10 ) 
    s << "0";
  s << seconds << ":";
  if( hundreds < 10 ) 
    s << "0";
  s << hundreds;
  return s.str();
}

string 
Timer::getTimeMMSS() {
  if( paused )
    return "PAUSED";
	stringstream s;
	int seconds = getTime() / 1000;
	int minutes = seconds / 60;
	seconds = seconds % 60;
	if( minutes < 10 )
		s << "0";
	s << minutes << ":";
	if( seconds < 10 ) 
		s << "0";
	s << seconds;
	return s.str();
}



bool
Timer::isPaused() {
  return paused;
}
