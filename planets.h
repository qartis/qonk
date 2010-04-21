// Copyright 2005 by Anthony Liekens anthony@liekens.net

#ifndef PLANETS_H
#define PLANETS_H

#include "coordinate.h"
#include "ships.h"
#include "extensions.h"
#include "selection.h"
#include "actions.h"
#include "config.h"
#include "SDL.h"
#include <sstream>
#include <list>
using namespace std;

class Ship;
class Ships;
class Player;
class Players;
class Universe;

class Planet {
private:
public:
	Universe* universe;
	Player* owner;
	int rotationSpeed, rotationOffset;
	double rotationDistance;
	int shipCreationSpeed;
	int size;
	bool isAMoon, selected;
	Planet* mother;
	list< Ship* > residentShips;
	Uint32 buildStartTime, buildEndTime;

	Planet(); // make a planet
	void makeMoon( Planet* );
	Coordinate getVector( Uint32 time ) const;
	Coordinate getLocation() const;
	Coordinate getLocation( Uint32 time ) const;
	
	void addResident( Ship* ship, Uint32 time );
	void createShip( const Uint32& time, Player* player );
	void removeResident( Ship* ship );
	
	void render( SDL_Surface* screen ) const;
	//void renderOrbit( SDL_Surface* screen ) const;
	void renderSelector( SDL_Surface* screen ) const;
	void renderBuildProgress( SDL_Surface* screen ) const;
	
	void updateShipLocations();
	
	double distance( Planet* planet );
	
	int numberOfResidentShips() { return residentShips.size(); }
	void setSelected( bool selected );
	void setUniverse( Universe* universe );
	bool getMoon() const { return isAMoon; }
	Player* getOwner() { return owner; }
};

class Planets : public std::list< Planet > { 
public:
	Planets();
	Planets( int numberOfPlanets, int numberOfMoons );
	
	void addPlanets( int numberOfPlanets );
	void addMoons( int numberOfMoons );

	void render( SDL_Surface* screen ) const;
	//void renderOrbits( SDL_Surface* screen ) const;
	
	Planet* closestToCoordinate( const Coordinate& c );
	Planet* closestToCoordinate( const Coordinate& c, double treshold );
	
	void select( Selection selection );
	
	void updateShipLocations();
	
	void setUniverse( Universe* universe );
	
	Planet* getRandomPlanet();
	Planet* getRandomEnemyPlanet( Player* );
	Planet* getRandomFriendlyPlanet( Player* );
	Planet* getRandomNearbyPlanet( Planet* );
	
};

class CreateShipAction : public Action {
private:
	Player* player;
	Planet* planet;
public:
	CreateShipAction();
	CreateShipAction( Player* player, Planet* planet );
	void execute( const Uint32& time );
	Planet* getPlanet() const;
};

#endif
