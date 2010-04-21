// Copyright 2005 by Anthony Liekens anthony@liekens.net

#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include "config.h"
#include "SDL.h"
#include <sstream>
using namespace std;

void alphaBlend( SDL_Surface* surface, Uint8 alpha );

void drawSelector( SDL_Surface *screen, Sint16 x1, Sint16 y1, Sint16 width, Sint16 height, Uint8 R, Uint8 G, Uint8 B );
void drawPlanet( SDL_Surface* screen, Sint16 x, Sint16 y, int size, Uint8 R, Uint8 G, Uint8 B );
void drawPlanet( SDL_Surface* screen, Sint16 x, Sint16 y, int size, Uint32 color );
double frand();
double frand( double max );
Uint32 HSVtoRGB( float h, float s, float v );

Uint8 getRed( Uint32 color );
Uint8 getGreen( Uint32 color );
Uint8 getBlue( Uint32 color );

#endif
