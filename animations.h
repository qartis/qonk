// Copyright 2005 by Anthony Liekens anthony@liekens.net

#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include "actions.h"
#include "coordinate.h"
#include "config.h"
#include "planets.h"

#include <math.h>

class Planet;

class Animation : public Action {
public:
  void execute(__attribute__((unused)) const Uint32& time ) {}
  virtual void render( SDL_Surface* screen ) = 0;
};

class AnimationQueue : public ActionQueue {
public:
  void render( SDL_Surface* screen );
};

class SonarAnimation : public Animation {
private:
  bool circle;
  Planet* planet;
  Uint32 startTime, endTime;
  Uint8 r, g, b;
  int size;
public:
  SonarAnimation();
  SonarAnimation( Planet* planet, const Uint32& color, const int& size, const Uint32& startTime, const Uint32& endTime, bool circle = false );
  void render( SDL_Surface* screen );
};

#endif
