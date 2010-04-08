// Copyright 2005 by Anthony Liekens anthony@liekens.net

#ifndef PLAYERS_H
#define PLAYERS_H

#include "ships.h"
#include "planets.h"
#include "universe.h"
#include "actions.h"
#include <list>
using namespace std;

class Ship;
class Ships;
class Planet;
class Planets;
class Selection;
class Universe;

class Player {
public:
  enum PlayerTypes { HUMAN, COMPUTER, NEUTRAL };
  enum PlayerStates { ALIVE, DEAD };
protected:
  Universe* universe;
  Ships* ships;
  PlayerTypes playerType;
  PlayerStates playerState;
  Uint32 color;
  vector< int > stats;
public:
  Player();
  Player( Universe* universe, Planet* homePlanet, int numberOfShips, Uint32 color );
  virtual ~Player() {}
  
  void addShips( Planet* planet, int numberOfShips );
  void createShip( Uint32& time, Planet* planet );
  void removeDeadShips();
  
  virtual void render( SDL_Surface* screen ) = 0;
  void updateStats( double maximumPoints );
  bool renderStats( SDL_Surface* screen, int height );
  void renderStatsLog( SDL_Surface* screen );
  virtual void update() = 0;
  
  PlayerTypes getPlayerType();
  
  void setColor( const Uint32& );
  Uint32 getColor();
  Universe* getUniverse() { return universe; }
  double getPoints();
  
};

class HumanPlayer : public Player {
private:
  Selection* mouseSelection;
  bool rightMouseButtonPushed;
public:
  HumanPlayer( Universe* universe, Planet* homePlanet, int numberOfShips, Uint32 color );
  
  void render( SDL_Surface* screen );
  
  void update();
  
  void selectAllShips();
};

class ComputerPlayer : public Player {
private:
  // strategy related
  
  double proportionResidents; // proportion of all ships to reside on planets
  double moonPriority, enemyPriority, preferredPriority, neutralPriority, weakerEnemyPriority, strongerEnemyPriority; 
  
  bool displayShips;
  
  list< Planet* > preferredPlanets;

public:
  ComputerPlayer( Universe* universe, Planet* homePlanet, int numberOfShips, Uint32 color );
  
  void render( SDL_Surface* screen );

  void update();
  
  void action( const Uint32& time );
  
  void toggleDisplayShips();
  void setDisplayShips( bool );
  bool isAPreferredPlanet( Planet* );
  Planet* getRandomNearbyPlanet( Planet* );
  void printStats();
};

class NeutralPlayer : public Player {
private:
  bool displayShips;
public:
  NeutralPlayer( Universe* universe, Planet* homePlanet, int numberOfShips, Uint32 color );
  
  void render( SDL_Surface* screen );

  void update();
  
  void toggleDisplayShips();
  void setDisplayShips( bool );
};

class Players : public list< Player* > {
protected:
  Universe* universe;
  Player* bestPlayer;
public:
  Players( Universe* universe );

  void render( SDL_Surface* screen );
  double getSumOfScores();
  void updateStats( int time );
  void renderStats( SDL_Surface* screen );
  
  void update();
};

class ComputerPlayerAction : public Action {
private:
  ComputerPlayer* player;
public:
  ComputerPlayerAction();
  ComputerPlayerAction( ComputerPlayer* player );
  void execute( const Uint32& time );
};

class UpdatePlayersStatsAction : public Action {
private:
  Players* players;
public:
  UpdatePlayersStatsAction();
  UpdatePlayersStatsAction( Players* players );
  void execute( const Uint32& time );
};

#endif
