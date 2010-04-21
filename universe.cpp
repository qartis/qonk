// Copyright 2005 by Anthony Liekens anthony@liekens.net

#include "universe.h"
#include "SDL_gfxPrimitives.h"
#include <iostream>

using namespace std;

Universe::Universe( int nrPlanets ) {
    stars = NULL;
    if (!is_server){
        stars = new Stars( 100 );
    }
    planets = new Planets( nrPlanets, nrPlanets );
    planets->setUniverse( this );
    actionQueue = new ActionQueue();
    animationQueue = new AnimationQueue();
    messages = new Messages();
}

Universe::~Universe() {
	delete stars;
	delete planets;
	delete actionQueue;
	delete animationQueue;
	delete messages;
}

void Universe::update() {
  actionQueue->executeEventsBefore( timer.getTime() );
  animationQueue->executeEventsBefore( timer.getTime() );
}

void Universe::renderBackground( SDL_Surface* screen ) {
    if (!is_server){
        stars->render( screen );
    }
	
	messages->render( screen );
	
	int radarSteps = 4;
	//Uint32 radarColor = 0xfee190;
	double s = 1.0 / radarSteps;
	for( int i = 1; i <= radarSteps; i++ ) {
		aaellipseRGBA( screen, settings.getGameOffsetX() + settings.getGameWidth() / 2, settings.getGameHeight() / 2, (int)( i * s * settings.getGameWidth() / 2 ), (int)( i * s * settings.getGameHeight() / 2 ), 255, 225, 144, 64 );
	}
	lineRGBA( screen, settings.getGameOffsetX() + settings.getGameWidth() / 2, 0, settings.getGameOffsetX() + settings.getGameWidth() / 2, settings.getGameHeight(), 255, 225, 144, 64 );
	lineRGBA( screen, settings.getGameOffsetX(), settings.getGameHeight() / 2, settings.getGameOffsetX() + settings.getGameWidth(), settings.getGameHeight() / 2, 255, 225, 144, 64 );
	
	int mouseX;
	int mouseY;
	Uint8 mouseState = SDL_GetMouseState( &mouseX, &mouseY );
	double pointerX = ( (double)mouseX - settings.getGameOffsetX() ) / settings.getGameWidth();
	double pointerY = (double)mouseY / settings.getGameHeight();
	Planet* closestPlanet = planets->closestToCoordinate( Coordinate( pointerX, pointerY ), 1 );

	if( !( mouseState & SDL_BUTTON(1) ) ) {

        /*
		if( closestPlanet != NULL ) {
			closestPlanet->renderOrbit( screen );
		}
		*/
		currentSelectedPlanet = closestPlanet;
			
	}
	
	animationQueue->render( screen );

	filledCircleRGBA( screen, settings.getGameOffsetX() + settings.getGameWidth() / 2, settings.getGameHeight() / 2, 7, 255, 239, 191, 255 );
	aacircleRGBA( screen, settings.getGameOffsetX() + settings.getGameWidth() / 2, settings.getGameHeight() / 2, 7, 255, 239, 191, 255 );

}

void Universe::renderForeground( SDL_Surface* screen ) {
	planets->updateShipLocations();
	planets->render( screen );

	if( currentSelectedPlanet != NULL ) {
		currentSelectedPlanet->renderSelector( screen );
	}
}

