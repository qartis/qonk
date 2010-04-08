// Copyright 2005 by Anthony Liekens anthony@liekens.net

#ifndef STARS_H
#define STARS_H

#include "SDL_gfxPrimitives.h"
#include "coordinate.h"
#include <SDL.h>
#include <vector>
#include <stdlib.h>

class Star {
private:
	Coordinate c;
	Uint8 brightness;
public:
	Star();
	void render( SDL_Surface* screen ) const;
};

class Stars : public std::vector< Star > {
public:
	Stars();
	Stars( int n );
	
	void addStars( int n );
	
	void render( SDL_Surface* screen ) const;
};

#endif
