// Copyirght 2005 by Anthony Liekens anthony@liekens.net

#include "config.h"
#include <iostream>
using namespace std;

Settings::Settings() {
  setResolution( 640,480 );
  setFullscreen( false );
//  setResolution( 1280,1024 );
//  setFullscreen( true );
}

void Settings::setResolution( int width, int height ) {
  screenWidth = width;
  screenHeight = height;
  gameWidth = height;
  gameHeight = height;
  gameOffsetX = ( width - height ) / 2;
}

void Settings::setFullscreen( bool b ) {
  fullscreen = b;
}

int Settings::getScreenWidth() { 
  return screenWidth; 
}

int Settings::getScreenHeight() { 
  return screenHeight; 
}

int Settings::getGameWidth() { 
  return gameWidth; 
}

int Settings::getGameHeight() { 
  return gameHeight; 
}

int Settings::getGameOffsetX() { 
  return gameOffsetX; 
}

bool Settings::getFullscreen() { 
  return fullscreen; 
}
