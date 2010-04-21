// Copyright 2005 by Anthony Liekens anthony@liekens.net

#include "planets.h"

#include "SDL_gfxPrimitives.h"

#include <iostream>
#include <math.h>

// ##### PLANET #####

Planet::Planet() {
	rotationSpeed = ( ( rand() % 2 ) * 2 - 1 ) * ( rand() % 60000 + 20000 );
	rotationDistance = 0.05 + 0.4 * ( double )rand() / RAND_MAX;
	rotationOffset = rand() % 10000000;
	size = 3;
	isAMoon = false;
	mother = NULL;
	selected = false;
	owner = NULL;
	shipCreationSpeed = 10000 + rand() % 5000;
}

void
Planet::makeMoon( Planet* mother ) {
	rotationSpeed = ( ( rand() % 2 ) * 2 - 1 ) * ( rand() % 2000 + 2000 );
	rotationDistance = 0.01 + 0.03 * ( double )rand() / RAND_MAX;
	rotationOffset = rand() % 100000;
	size = 2;
	isAMoon = true;
	this->mother = mother;
	shipCreationSpeed = 20000 + rand() % 10000;
}

Coordinate
Planet::getVector( Uint32 time ) const {
	double angle = (double)( rotationOffset + time ) / rotationSpeed;
	return Coordinate( cos( angle ), sin( angle ) );
}

Coordinate
Planet::getLocation( Uint32 time ) const {
	Coordinate motherLocation;
	if( isAMoon ) {
		motherLocation = mother->getLocation();
	} else {
		motherLocation.setX( 0.5 );
		motherLocation.setY( 0.5 );
	}
	Coordinate v = getVector( time );
	double x = motherLocation.getX() + rotationDistance * v.getX();
	double y = motherLocation.getY() + rotationDistance * v.getY();
	return Coordinate( x, y );
}

Coordinate
Planet::getLocation() const {
	return getLocation( timer.getTime() );
}

void
Planet::addResident( Ship* ship, Uint32 time ) {
	if( ( owner == NULL ) || ( owner == ship->getOwner() ) || ( residentShips.size() == 0 ) ) {
		if( owner != ship->getOwner() ) {
		
			// clear out "create ship" actions scheduled for this planet
			vector< ActionQueue::iterator > removes;
			for( ActionQueue::iterator i = universe->actionQueue->begin(); i != universe->actionQueue->end(); i++ ) {
				if( (i->second)->getActionID() == 1 ) {
					Planet* planetOfIterator = ((CreateShipAction*)(i->second))->getPlanet();
					if( planetOfIterator == this )
						removes.push_back( i );
				}
			}
			
			for( vector< ActionQueue::iterator >::iterator i = removes.begin(); i != removes.end(); i++ ) {
				universe->actionQueue->erase( *i );
			}
			
			// create a new "create ship" action
			buildStartTime = time;
			buildEndTime = time + shipCreationSpeed;
	    universe->actionQueue->scheduleAction( buildEndTime, new CreateShipAction( ship->getOwner(), this ) );
	    
			// show an animation of planet conquests that are of our interest
			Uint32 color = 0;
			color = ship->getOwner()->getColor();
            if( owner != NULL ){
                if( isAMoon ){
                    universe->animationQueue->scheduleAction( time + 1200, new SonarAnimation( this, color, 50, time, time + 1000, true ) );
                } else {
                    universe->animationQueue->scheduleAction( time + 1200, new SonarAnimation( this, color, 100, time, time + 1000, true ) );
                }
            }
			
    }
		residentShips.push_back( ship );
		owner = ship->getOwner();
		
	} else {
		ship->die();
		Ship* resident = *( residentShips.begin() );
		removeResident( resident );
		resident->die();
		
		// show an animation of die'ing ships that are of our interest
		if( ( ship->getOwner()->getPlayerType() == Player::HUMAN ) || ( owner->getPlayerType() == Player::HUMAN ) ) {
			universe->animationQueue->scheduleAction( time + 1000, new SonarAnimation( this, ship->getOwner()->getColor(), 20, time, time + 1000, true ) );
		}
//		if( owner->getPlayerType() == Player::HUMAN ) {
//			universe->animationQueue->scheduleAction( time + 1000, new SonarAnimation( this, 0xc00000, 20, time, time + 1000 ) );
//		}
	}
}

void
Planet::removeResident( Ship* ship ) {
	residentShips.remove( ship );
}

void
Planet::render( SDL_Surface* screen ) const {
	if( owner->getPlayerType() == Player::HUMAN )
		renderBuildProgress( screen );
	Coordinate location = getLocation();
	int x = location.getXMapped();
	int y = location.getYMapped();
	drawPlanet( screen, x, y, size, owner->getColor() );
	if( selected )
		renderSelector( screen );
}

/* void Planet::renderOrbit( SDL_Surface* screen ) const {
	Coordinate centerLocation;
	if( isAMoon ) {
		centerLocation = mother->getLocation();
	} else {
		centerLocation.setX( 0.5 );
		centerLocation.setY( 0.5 );
	}
	if( owner != NULL )
		aaellipseRGBA( screen, centerLocation.getXMapped(), centerLocation.getYMapped(), (int)( rotationDistance * settings.getGameWidth() ), (int)( rotationDistance * settings.getGameHeight() ), getRed( owner->getColor() ), getGreen( owner->getColor() ), getBlue( owner->getColor() ), 64 );
	if( getMoon() ) {
		mother->renderOrbit( screen );
	}
} */

void
Planet::renderSelector( SDL_Surface* screen ) const {
	Coordinate location = getLocation();
	int x = location.getXMapped();
	int y = location.getYMapped();
	drawSelector( screen, x - 4, y - 4, 10, 10, 0, 0, 0 );
	drawSelector( screen, x - 5, y - 5, 10, 10, 255, 192, 0 );
}

void
Planet::renderBuildProgress( SDL_Surface* screen ) const {


	Coordinate location = getLocation();
	//Font* font = universe->messages->getFont();
	int x = location.getXMapped();
	int y = location.getYMapped();
	double percentage = 100.0 * ( timer.getTime() - buildStartTime ) / ( buildEndTime - buildStartTime );
	aacircleRGBA( screen, x, y, (int)( size + 102.5 - percentage ), 0xff, 0xff, 0xff, (int)( 0.2 * percentage ) );

}

void
Planet::updateShipLocations() {
	if( residentShips.size() > 0 ) {
		Coordinate location = getLocation();
		int counter = 0;
		double offset = 500 * rotationDistance + (double)timer.getTime() / 10000;
		for( list< Ship* >::iterator i = residentShips.begin(); i != residentShips.end(); i++ ) {
			double shipX = location.getX() + 0.02 * cos( offset + counter * 2 * PI / residentShips.size() );
			double shipY = location.getY() + 0.02 * sin( offset + counter * 2 * PI / residentShips.size() );
			(*i)->setLocation( shipX, shipY );
			counter++;
		}
	}
}

double
Planet::distance( Planet* planet ) {
	return planet->getLocation().distance( getLocation() );
}

void
Planet::setSelected( bool selected ) {
	this->selected = selected;
}

void
Planet::setUniverse( Universe* universe ) {
	this->universe = universe;
}

void
Planet::createShip( const Uint32& time, Player* player ) {
	if( player->getPlayerType() == Player::NEUTRAL ){
        return;
    }

    if( owner == player ) {
        player->addShips( this, 1 );
        buildStartTime = time;
        buildEndTime = time + shipCreationSpeed;
        universe->actionQueue->scheduleAction( buildEndTime, new CreateShipAction( player, this ) );
    }
}

// ##### PLANETS #####

Planets::Planets() {
}

Planets::Planets( int numberOfPlanets, int numberOfMoons ) {

	addPlanets( numberOfPlanets );
	addMoons( numberOfMoons );
	
}

void Planets::addPlanets( int numberOfPlanets ) {
	for( int i = 0; i < numberOfPlanets; i++ ) {
		push_back( Planet() );
	}
}

void Planets::addMoons( int numberOfMoons ) {

	if( size() == 0 ) {
		cerr << "Error constructing moons, no planets yet" << endl;
		exit( 1 );
	}

	for( int i = 0; i < numberOfMoons; i++ ) {
		iterator motherPlanet;
		bool done = false;
		do {
			int mother = rand() % size();
			motherPlanet = begin();
			for( int j = 0; j < mother; j++ ) motherPlanet++;
			done = !motherPlanet->getMoon();
		} while( !done );
		Planet p;
		p.makeMoon( &(*motherPlanet) );
		push_back( p );
	}
	
}

void Planets::render( SDL_Surface* screen ) const {
	for( const_iterator i = begin(); i != end(); i++ )
		i->render( screen );
}

/*
void Planets::renderOrbits( SDL_Surface* screen ) const {
	for( const_iterator i = begin(); i != end(); i++ )
		i->renderOrbit( screen );
}*/

Planet* Planets::closestToCoordinate( const Coordinate& c ) {
	double closestDistance = 5000;
	Planet* closestPlanet = NULL;
	for( iterator i = begin(); i != end(); i++ ) {
		double distance = i->getLocation().distance( c );
		if( distance < closestDistance ) {
			closestDistance = distance;
			closestPlanet = &(*i);
		}
	}
	return closestPlanet;
}

Planet* Planets::closestToCoordinate( const Coordinate& c, double treshold ) {
	Planet* closest = closestToCoordinate( c );
	if( closest->getLocation().distance( c ) < treshold )
		return closest;
	else
		return NULL;
}

void
Planets::select( Selection selection ) {

	double minX = selection.getMinX();
	double maxX = selection.getMaxX();
	double minY = selection.getMinY();
	double maxY = selection.getMaxY();
	
	for( iterator i = begin(); i != end(); i++ ) {
		Coordinate location = i->getLocation();
		if( ( location.getX() > minX ) && ( location.getX() < maxX ) && ( location.getY() > minY ) && ( location.getY() < maxY ) )
			i->setSelected( true );
		else
			i->setSelected( false );
	}
	
}

void 
Planets::updateShipLocations() {
	for( iterator i = begin(); i != end(); i++ )
		i->updateShipLocations();
}

void
Planets::setUniverse( Universe* universe ) {
	for( iterator i = begin(); i != end(); i++ )
		i->setUniverse( universe );
}

Planet*
Planets::getRandomPlanet() {
	int number = rand() % size();
	iterator i = begin();
	for( int k = 0; k < number; k++ )
		i++;
	return &(*i);
}

Planet*
Planets::getRandomEnemyPlanet( Player* player ) {
    //XXX
    return NULL;
}

Planet*
Planets::getRandomNearbyPlanet( Planet* planet ) {

	Planet* result;
	do {
		result = getRandomPlanet();
	} while( result == planet );
	double distance = planet->distance( result );
	
	for( unsigned int i = 0; i < size() / 2; i++ ) {
		Planet* otherPlanet;
		do {
			otherPlanet = getRandomPlanet();
		} while( otherPlanet == planet );
		double otherDistance = planet->distance( otherPlanet );
		
		// which planets do we prefer?
		
		if( otherPlanet->getOwner() != planet->getOwner() )
			otherDistance *= 0.1;
		if( otherPlanet->getOwner()->getPlayerType() == Player::NEUTRAL )
			otherDistance *= 0.5;
		if( ! otherPlanet->getMoon() )
			otherDistance *= 0.8;
		
		if( otherDistance < distance ) {
			result = otherPlanet;
			distance = otherDistance;
		}
	}
	
	return result;

}

// ##### CREATESHIPACTION #####

CreateShipAction::CreateShipAction() : Action() {
	actionID = 1;
}

CreateShipAction::CreateShipAction( Player* player, Planet* planet ) {
	this->player = player;
	this->planet = planet;
	actionID = 1;
}

void
CreateShipAction::execute( const Uint32& time ) {
	planet->createShip( time, player );
}

Planet*
CreateShipAction::getPlanet() const {
	return planet;
}
