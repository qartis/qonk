// Copyright 2005 by Anthony Liekens anthony@liekens.net

#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "stars.h"
#include "planets.h"
#include "actions.h"
#include "animations.h"
#include "messages.h"

class Stars;
class Planet;
class Planets;
class AnimationQueue;

class Universe {
public:
  Stars* stars;
  Planets* planets;
  ActionQueue* actionQueue;
  AnimationQueue* animationQueue;
  Messages* messages;
  
  Planet* currentSelectedPlanet;
  
  Universe( int nrPlanets );
  ~Universe();
  
  void update();
  
  void renderBackground( SDL_Surface* screen );
  void renderForeground( SDL_Surface* screen );
};

#endif
