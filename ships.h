// Copyright 2005 by Anthony Liekens anthony@liekens.net

#ifndef SHIPS_H
#define SHIPS_H

#include "players.h"
#include "planets.h"
#include "actions.h"
#include "selection.h"
#include "SDL.h"
#include <list>
using namespace std;

class Planet;

class Ship {
public:
  enum ShipStates { RESIDENT, MOVING, DEAD };
  
private:
  Player* owner;
  Planet* planet;
  Coordinate location;
  bool selected;
  double speed;
  int id;
  
  ShipStates shipState;
  
  // for movement purposes only
  
	Coordinate fromLocation, toLocation;
	Uint32 fromTime, toTime;
	
public:
  Ship( Player*, Planet* );
  
  void die();
  
  void moveTo( Uint32, Planet*, ActionQueue* );
  
  void render( SDL_Surface* screen, Uint32 color ) const;
  void renderSelection( SDL_Surface* screen ) const;
  
  double distance( Ship* );
  
  void setLocation( double x, double y );
  void setLocation( const Coordinate& );
  Coordinate getLocation() const;
  ShipStates getShipState() const;
  void setSelected( bool selected );
  bool getSelected() const;
  double getSpeed() const;
  Player* getOwner() const;
  Planet* getPlanet() const;
};

class Ships : public list< Ship* > {
public:

  void removeDeadShips();

  void render( SDL_Surface* screen, Uint32 color ) const;
  
  void moveTo( Planet* destination, ActionQueue* actionQueue );
  int numberSelectedShips();

  void select( Selection* selection );
  void selectAll();
	
  int countResidents();
  Ship* getRandomShip();
  Ship* getRandomResidentShip();
  Ship* getRandomNearbyResidentShip( Ship* other );
  Ship* getNearestResidentShip( Planet* c );
};

class ShipMovementAction : public Action {
private:
	Ship* ship;
	Planet* destination;
public:
  ShipMovementAction();
  ShipMovementAction( Ship* ship, Planet* planet );
  void execute( const Uint32& time );
};

class DeadShipRemovalFunctor {
public:
  bool operator()(Ship* &s) {
    return s->getShipState() == Ship::DEAD;
  }
};

#endif
