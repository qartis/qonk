// Copyright 2005 by Anthony Liekens anthony@liekens.net

#include "players.h"

#include "SDL_gfxPrimitives.h"

#include <iostream>
#include <map>
using namespace std;

// ##### PLAYER #####

Player::Player() {
  ships = new Ships();
  playerState = ALIVE;
}

Player::Player( Universe* universe, Planet* homePlanet, int numberOfShips, Uint32 color ) {

  ships = new Ships();
  playerState = ALIVE;
  this->universe = universe;
  addShips( homePlanet, numberOfShips );
  setColor( color );
		
}

void Player::addShips( Planet* planet, int numberOfShips ) {
    for(int i = 0; i < numberOfShips; i++){
        ships->push_back( new Ship( this, planet ) );
        cout << "making ship" << endl;
    }
}

void Player::removeDeadShips() {
  ships->removeDeadShips();
}

void Player::updateStats( double maximumPoints ) {
  stats.push_back( (int)( getPoints() / maximumPoints * settings.getGameHeight()  + 0.5 ) );
  if( stats.size() == 1 ) {
    for( int i = 0; i < 127; i++ )
      stats.push_back( (int)( getPoints() / maximumPoints * settings.getGameHeight()  + 0.5 ) );
  }
  while( stats.size() > 128 ) {
    stats.erase( stats.begin() );
  }
}

bool
Player::renderStats( SDL_Surface* screen, int height ) {
  int counter = 4;
  for( Planets::iterator i = universe->planets->begin(); i != universe->planets->end(); i++ ) {
    if( i->getOwner() == this ) {
      int size = 3;
      if( i->getMoon() ) size = 2;
      drawPlanet( screen, counter, height + 5, size, color );
      counter+= size * 2 + 3;
    }
  }
//  if( getPoints() == 0 ) {
//    boxRGBA( screen, counter - 1, height + 2, counter + 1, height + 8, getRed( color ), getGreen( color ), getBlue( color ), 255 );
//    boxRGBA( screen, counter - 3, height + 4, counter + 3, height + 6, getRed( color ), getGreen( color ), getBlue( color ), 255 );
//    counter += 6;
//  }
  return true;
//  return ( counter != 4 );
}

void
Player::renderStatsLog( SDL_Surface* screen ) {
  if( playerType == HUMAN )
  for( unsigned int i = 1; i < stats.size(); i++ ) {
    aalineRGBA( screen, settings.getScreenWidth() - stats.size() + i - 1, settings.getGameHeight() - stats[ i - 1 ], settings.getScreenWidth() - stats.size() + i, settings.getGameHeight() - stats[ i ], getRed( color ), getGreen( color ), getBlue( color ), i * 2 );
  }
}

double
Player::getPoints() {
  double result = 0;
  for( Planets::iterator i = universe->planets->begin(); i != universe->planets->end(); i++ ) {
    if( i->getOwner() == this ) {
      if( i->getMoon() ) { 
        result += 0.999;
      } else {
        result += 2;
      }
    }
  }
  result += ships->size() / 3.0;
  return result;
}

Player::PlayerTypes
Player::getPlayerType() {
  return playerType;
}

void
Player::setColor( const Uint32& color ) {
  this->color = color;
}

Uint32
Player::getColor() {
  return color;
}

// ##### HUMANPLAYER #####

HumanPlayer::HumanPlayer( Universe* universe, Planet* homePlanet, int numberOfShips, Uint32 color ) :
Player( universe, homePlanet, numberOfShips, color ) {
  mouseSelection = new Selection();
  playerType = Player::HUMAN;
  universe->messages->addMessage( timer.getTime() + 000, Message( "Let's QONK! Kick the AI players out!", 15000, 0xffffff ) );
  universe->messages->addMessage( timer.getTime() + 200, Message( "[Click and drag] to select, [Right click] to send ships.", 10000, 0x808080 ) );
  universe->messages->addMessage( timer.getTime() + 400, Message( "[P]ause, [E]nemies, [A]ll ships.", 10000, 0x808080 ) );
}

void
HumanPlayer::render( SDL_Surface* screen ) {
  mouseSelection->render( screen );
  ships->render( screen, color );
}

void
HumanPlayer::update() {

  if( playerState == ALIVE ) {
    if( getPoints() == 0 ) {
      universe->messages->addMessage( timer.getTime(), Message( "GAME OVER!!! You lost all your planets and ships, you are dead!", 20000, color ) );
      universe->messages->addMessage( timer.getTime() + 200, Message( "Press [Escape] ...", 40000, 0x808080 ) );
      playerState = DEAD;
      return;
    }
  } else
    return;
    
  // update mouse selection, and select ships if left mouse button is released
  
	if( mouseSelection->update() ) {
		ships->select( mouseSelection );
	}
	
	// let ships move if the user requests this with right mouse button
	
	int mouseX;
	int mouseY;
	Uint8 mouseState = SDL_GetMouseState( &mouseX, &mouseY );
	if( mouseState & SDL_BUTTON( 3 ) ) {
	if( !rightMouseButtonPushed ) {
	  rightMouseButtonPushed = true;
          double pointerX = ( (double)mouseX - settings.getGameOffsetX() ) / settings.getGameWidth();
          double pointerY = (double)mouseY / settings.getGameHeight();
          Planet* closestPlanet = universe->planets->closestToCoordinate( Coordinate( pointerX, pointerY ), 1 );
          if( closestPlanet != NULL ) {
	    if( ships->numberSelectedShips() > 0 ) {
              ships->moveTo( closestPlanet, universe->actionQueue );
            } else {
              Ship* ship = ships->getNearestResidentShip( closestPlanet );
              if( ship != NULL ) {
                ship->moveTo( timer.getTime(), closestPlanet, universe->actionQueue );
              } else {
              }
            }
          }
	}
	} else rightMouseButtonPushed = false;
	
	removeDeadShips();
	
}

void
HumanPlayer::selectAllShips() {
  ships->selectAll();
}

// ##### COMPUTERPLAYER #####

ComputerPlayer::ComputerPlayer( Universe* universe, Planet* homePlanet, int numberOfShips, Uint32 color ) :
Player( universe, homePlanet, numberOfShips, color ) {
  playerType = Player::COMPUTER;
  
  // give the computer player some values for determining strategic actions
  
  proportionResidents = 0.3 + frand( 0.4 );
  moonPriority = 0.5 + frand( 0.7 );
  enemyPriority = 0.05 + frand( 0.45 );
  preferredPriority = 0.2 + frand( 0.5 );
  neutralPriority = 0.2 + frand( 0.6 );
  weakerEnemyPriority = 0.2 + frand( 1.6 );
  strongerEnemyPriority = 0.2 + frand( 1.6 );
  
  universe->actionQueue->scheduleAction( rand() % 10000, new ComputerPlayerAction( this ) );
  
  displayShips = false;
}

void 
ComputerPlayer::printStats() {
  cout << "proportionResidents:" << proportionResidents << " moonPriority:" << moonPriority << " enemyPriority:" << enemyPriority << " preferredPriority:" << preferredPriority << " neutralPriority:" << neutralPriority << endl;
}

void
ComputerPlayer::render( SDL_Surface* screen ) {
  if( displayShips )
    ships->render( screen, color );
}

void
ComputerPlayer::update() {
  if( playerState == ALIVE ) {
    if( getPoints() == 0 ) {
      universe->messages->addMessage( timer.getTime(), Message( "AI player is dead", getRed( color ), getGreen( color ), getBlue( color ) ) );
      playerState = DEAD;
      return;
    }
  } else
    return;
  removeDeadShips();
}

bool
ComputerPlayer::isAPreferredPlanet( Planet* planet ) {
  for( list< Planet* >::iterator i = preferredPlanets.begin(); i != preferredPlanets.end(); i++ )
    if( (*i) == planet )
      return true;
  return false;
}

Planet*
ComputerPlayer::getRandomNearbyPlanet( Planet* planet ) {

	Planet* result;
	do {
		result = universe->planets->getRandomPlanet();
	} while( result == planet );
	double distance = planet->distance( result );
	
	for( unsigned int i = 0; i < universe->planets->size() * 2; i++ ) {
		Planet* otherPlanet;
		do {
			otherPlanet = universe->planets->getRandomPlanet();
		} while( otherPlanet == planet );
		double otherDistance = planet->distance( otherPlanet );
		
		// which planets do we prefer?
		
		if( otherPlanet->getOwner() != planet->getOwner() ) {
			otherDistance *= enemyPriority;
			if( isAPreferredPlanet( otherPlanet ) ) {
			  otherDistance *= preferredPriority;
      }
      if( otherPlanet->getOwner()->getPlayerType() != Player::NEUTRAL ) {
        if( otherPlanet->getOwner()->getPoints() < getPoints() )
          otherDistance *= weakerEnemyPriority;
        else
          otherDistance *= strongerEnemyPriority;
      }
    }
		if( otherPlanet->getOwner()->getPlayerType() == Player::NEUTRAL )
			otherDistance *= neutralPriority;
		if( !otherPlanet->getMoon() )
			otherDistance *= moonPriority;
		
		if( otherDistance < distance ) {
			result = otherPlanet;
			distance = otherDistance;
		}
	}
	
	return result;

}



void
ComputerPlayer::action( const Uint32& time ) {

  // The AI of the computer player ... it's not much, but it does some
  // intelligent things
  // 
  // * compute the number of ships we want to launch
  // * divide this into a set of squadrons
  // * each squadron (nearby resident ships) launches to a nearby planet
  // * schedule our next activity
  
  // how many (resident) ships do i have, and how many do I want to launch
  // according to my strategy?
  
  //int numberOfShips = ships->size();
  int numberOfResidents = ships->countResidents();
  int numberOfLaunches = numberOfResidents - (int)( ships->size() * proportionResidents ) - 1;
  
  if( numberOfLaunches > 0 ) {
  
    Ship* ship;
    Planet* sourcePlanet;
    Planet* destinationPlanet;
    
    int squadronSize = rand() % numberOfLaunches + 1;
    if( squadronSize > 10 ) {
      squadronSize = 10;
    }
  
    ship = ships->getRandomResidentShip();

    sourcePlanet = ship->getPlanet(); 
    destinationPlanet = getRandomNearbyPlanet( sourcePlanet );
    if( ( destinationPlanet->getOwner() == this ) && ( destinationPlanet->numberOfResidentShips() > sourcePlanet->numberOfResidentShips() ) )
      destinationPlanet = sourcePlanet;
    
    if( !isAPreferredPlanet( destinationPlanet ) )
    if( destinationPlanet->getOwner() != this )
      preferredPlanets.push_back( destinationPlanet );
    if( preferredPlanets.size() > 3 )
      preferredPlanets.erase( preferredPlanets.begin() );
  
    int launchedShips = 0;
  
    for( int i = 0; i < squadronSize; i++ ) {
    
      Ship* nextShip = ships->getRandomNearbyResidentShip( ship );
    
      if( nextShip != NULL ) {
        if( nextShip->distance( ship ) < 0.25 ) {
          nextShip->moveTo( time + rand() % 500, destinationPlanet, universe->actionQueue );
          launchedShips++;
          ship = nextShip;
        }
      }
      
    }
    
    int wait = (int)( ( ( 1 + launchedShips ) / (double)numberOfLaunches ) * ( 2000 + rand() % 1000 ) );
    universe->actionQueue->scheduleAction( time + wait, new ComputerPlayerAction( this ) );
    return;

  }

  //int wait = 3000 + rand() % 1500;
  universe->actionQueue->scheduleAction( time + 4000 + rand() % 2000, new ComputerPlayerAction( this ) );
  
}

void
ComputerPlayer::toggleDisplayShips() {
  displayShips = !displayShips;
}

void
ComputerPlayer::setDisplayShips( bool b ) {
  displayShips = b;
}

// ##### NEUTRALPLAYER #####

NeutralPlayer::NeutralPlayer( Universe* universe, Planet* homePlanet, int numberOfShips, Uint32 color ) :
Player( universe, homePlanet, numberOfShips, color ) {
  playerType = Player::NEUTRAL;
  displayShips = false;
}

void
NeutralPlayer::render( SDL_Surface* screen ) {
  if( displayShips )
    ships->render( screen, color );
}

void
NeutralPlayer::update() {
  removeDeadShips();
}

void
NeutralPlayer::toggleDisplayShips() {
  displayShips = !displayShips;
}

void
NeutralPlayer::setDisplayShips( bool b ) {
  displayShips = b;
}

// ##### PLAYERS #####

Players::Players( Universe* universe ) {
  this->universe = universe;
  bestPlayer = NULL;
}

void
Players::render( SDL_Surface* screen ) {
  for( iterator i = begin(); i != end(); i++ ) {
    (*i)->render( screen );
  }
  renderStats( screen );
}

double
Players::getSumOfScores() {
  double sum = 0;
  for( iterator i = begin(); i != end(); i++ ) {
    if( (*i)->getPlayerType() != Player::NEUTRAL )
      sum += (*i)->getPoints();
  }
  return sum;
}

void
Players::updateStats( int time ) {
  double sumOfScores = getSumOfScores();
  for( iterator i = begin(); i != end(); i++ ) {
    (*i)->updateStats( sumOfScores );
  }
  universe->actionQueue->scheduleAction( time + 1000, new UpdatePlayersStatsAction( this ) );
}

void
Players::renderStats( SDL_Surface* screen ) {

  multimap< double, Player* > playerSorter;
  for( iterator i = begin(); i != end(); i++ ) {
    playerSorter.insert( pair< double, Player* >( (*i)->getPoints(), *i ) );
    (*i)->renderStatsLog( screen );
  }
  
  Player* currentBestPlayer = playerSorter.rbegin()->second;
  //double maxPoints = currentBestPlayer->getPoints();
  if( currentBestPlayer != bestPlayer ) {
    bestPlayer = currentBestPlayer;
    if( bestPlayer->getPlayerType() == Player::COMPUTER )
      universe->messages->addMessage( timer.getTime(), Message( "AI player takes the lead", 10000, getRed( bestPlayer->getColor() ), getGreen( bestPlayer->getColor() ), getBlue( bestPlayer->getColor() ) ) );
    else if( bestPlayer->getPlayerType() == Player::HUMAN )
      universe->messages->addMessage( timer.getTime(), Message( "You take the lead", 10000, getRed( bestPlayer->getColor() ), getGreen( bestPlayer->getColor() ), getBlue( bestPlayer->getColor() ) ) );
  }

  int height = 0;
  for( multimap< double, Player* >::reverse_iterator i = playerSorter.rbegin(); i != playerSorter.rend(); i++ ) {
    Player* p = i->second;
    if( p->getPlayerType() != Player::NEUTRAL ) {
      if( p->renderStats( screen, height ) )
        height += 10;
    }
  }
        
}

void
Players::update() {
  for( iterator i = begin(); i != end(); i++ ) {
    (*i)->update();
  }
}

// ##### COMPUTERPLAYERACTION #####

ComputerPlayerAction::ComputerPlayerAction() {
}

ComputerPlayerAction::ComputerPlayerAction( ComputerPlayer* player ) {
  this->player = player;
}

void
ComputerPlayerAction::execute( const Uint32& time ) {
  player->action( time );
}

// ##### UPDATEPLAYERSSTATS #####

UpdatePlayersStatsAction::UpdatePlayersStatsAction() {
}

UpdatePlayersStatsAction::UpdatePlayersStatsAction( Players* players ) {
  this->players = players;
}

void
UpdatePlayersStatsAction::execute( const Uint32& time ) {
  players->updateStats( time );
}
