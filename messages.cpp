// Copyright 2005 by Anthony Liekens anthony@liekens.net

#include "messages.h"

Message::Message( string message, Uint32 displayTime, Uint8 r, Uint8 g, Uint8 b ) {
  this->message = string( "[" ) + timer.getTimeMMSS() + "] " + message;
  this->displayTime= displayTime;
  this->r = r;
  this->g = g;
  this->b = b;
}

Message::Message( string message, Uint32 displayTime, Uint32 color ) {
  this->message = string( "[" ) + timer.getTimeMMSS() + "] " + message;
  this->displayTime = displayTime;
  this->r = getGreen( color );
  this->g = getRed( color );
  this->b = getBlue( color );
}

Message::Message( string message, Uint8 r, Uint8 g, Uint8 b ) {
  this->message = string( "[" ) + timer.getTimeMMSS() + "] " + message;
  this->displayTime= 10000;
  this->r = r;
  this->g = g;
  this->b = b;
}

Message::Message( string message, Uint32 color ) {
  this->message = string( "[" ) + timer.getTimeMMSS() + "] " + message;
  this->displayTime = 10000;
  this->r = getGreen( color );
  this->g = getRed( color );
  this->b = getBlue( color );
}

void Message::render( SDL_Surface* screen, int &x, int &y, Font* font, unsigned int time ) {
    int dx = font->getHeight();
    int alpha = 255;

    // fading in a message
    if(time < 500) {
        dx = (int)( ( dx * time ) / 600.0 );
        alpha = (int)( ( alpha * time ) / 600.0 );
    }

    // fading out a message
    int timeleft = displayTime - time;
    if( timeleft < 500 ) {
        dx = (int)( ( dx * timeleft ) / 600.0 );
        alpha = (int)( ( alpha * timeleft ) / 600.0 );
    }

    // do we still show ourselves?
    if( time > displayTime ) {
        dx = 0;
        alpha = 0;
    } else {
        font->render( screen, x, y - dx, message.c_str(), r, g, b, alpha );
    }

    y -= dx;
}

Uint32 Message::getDisplayTime() {
  return displayTime;
}

Messages::Messages() {
  font = new Font( "bold.ttf", 18 );
}

Messages::~Messages() {
  clear();
}

void Messages::addMessage( Uint32 time, Message m ) {
  insert( pair< Uint32, Message>( time, m ) );
}

void Messages::addMessage( Message m ) {
  insert( pair< Uint32, Message>( timer.getTime() + 200, m ) );
}

void Messages::cleanup() {
  while( ( size() > 0 ) && ( begin()->first + begin()->second.getDisplayTime() < timer.getTime() ) )
    erase( begin() );
}

void Messages::render(SDL_Surface* screen) {
  cleanup();
  int x = 12; 
  int y = settings.getGameHeight() - 28;
  for( reverse_iterator i = rbegin(); i != rend(); i++ ) {
    i->second.render( screen, x, y, font, (int)timer.getTime() - (int)i->first );
  }
}
