// Copyright 2005 by Anthony Liekens anthony@liekens.net

#ifndef CONFIG_H
#define CONFIG_H

#define PI 3.14159265

#include "timer.h"
#include "debug.h"

static Timer timer;
extern bool is_server;

class Settings {
private:
  int screenWidth, screenHeight, gameWidth, gameHeight, gameOffsetX;
  bool fullscreen;
public:
  Settings();
  void setResolution( int width, int height );
  void setFullscreen( bool b );
  int getScreenWidth();
  int getScreenHeight();
  int getGameWidth();
  int getGameHeight();
  int getGameOffsetX();
  bool getFullscreen();
};

static Settings settings;

#endif
