// Copyright 2005 by Anthony Liekens anthony@liekens.net

#include "selection.h"
#include "SDL_gfxPrimitives.h"
#include <iostream>

Selection::Selection() {
	state = NOT_SELECTING;
}

bool Selection::update() {
	int mouseX, mouseY;
	Uint8 mouseState = SDL_GetMouseState( &mouseX, &mouseY );
	
	if( state == SELECTING ) {
		if( !( mouseState & SDL_BUTTON(1) ) ) {
			state = NOT_SELECTING;
			return true;
		} else {
			c2.setXMapped( mouseX );
			c2.setYMapped( mouseY );
		}
	} else {
		if( mouseState & SDL_BUTTON(1) ) {
			state = SELECTING;
			c1.setXMapped( mouseX );
			c1.setYMapped( mouseY );
			c2.setXMapped( mouseX );
			c2.setYMapped( mouseY );
		}
	}
	
	return false;
}

void Selection::render( SDL_Surface *screen ) {
	if( state == SELECTING ) {
		rectangleColor( screen, c1.getXMapped(), c1.getYMapped(), c2.getXMapped(), c2.getYMapped(), 0xfee19080 );
	}
}

double Selection::getMinX() const { 
  return ( c1.getX() < c2.getX() ? c1.getX() : c2.getX() );
}

double Selection::getMaxX() const { 
  return ( c1.getX() > c2.getX() ? c1.getX() : c2.getX() ); 
}

double Selection::getMinY() const { 
  return ( c1.getY() < c2.getY() ? c1.getY() : c2.getY() ); 
}

double Selection::getMaxY() const { 
  return ( c1.getY() > c2.getY() ? c1.getY() : c2.getY() ); 
}
