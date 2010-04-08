// Copyright 2005 by Anthony Liekens anthony@liekens.net

#ifndef MESSAGES_H
#define MESSAGES_H

#include "fonts.h"
#include "extensions.h"
#include "config.h"
#include "SDL.h"
#include <map>
#include <string>
using namespace std;

class Message {
private:
  Uint8 r, g, b;
  Uint32 displayTime;
  string message;
public:
  Message( string message, Uint32 displayTime, Uint8 r, Uint8 g, Uint8 b );
  Message( string message, Uint32 displayTime, Uint32 color );
  Message( string message, Uint8 r, Uint8 g, Uint8 b );
  Message( string message, Uint32 color );
  void render( SDL_Surface* screen, int &x, int &y, Font* f, unsigned int time );
  Uint32 getDisplayTime();
};

class Messages : multimap< Uint32, Message > {
private:
  Font* font;
public:
  Messages();
  ~Messages();
  void addMessage( Uint32 time, Message m );
  void cleanup();
  void render( SDL_Surface* screen );
  Font* getFont() { return font; }
};

#endif
