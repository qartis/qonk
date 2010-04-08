// Copyright 2005 by Anthony Liekens anthony@liekens.net

#include "actions.h"

#include "config.h"
#include <iostream>
using namespace std;

// ##### ACTION #####

Action::Action() {
	actionID = 0;
}

void
Action::setActionQueue( ActionQueue* actionQueue ) {
	this->actionQueue = actionQueue;
}

ActionQueue*
Action::getActionQueue() const {
	return actionQueue;
}

// ##### ACTIONQUEUE #####

ActionQueue::ActionQueue() {
	clear();
	time = 0;
}

void
ActionQueue::scheduleAction( Uint32 time, Action* action ) {
	if( this->time > time ) {
		std::cerr << "Action was inserted before action that has already been executed" << endl;
	}
	action->setActionQueue( this );
  insert( pair< Uint32, Action* >( time, action ) );
}

Uint32
ActionQueue::timeOfNextEvent() {
  return this->begin()->first;
}

void
ActionQueue::executeNextEvent() {
  time = this->begin()->first;
  Action* a = this->begin()->second;
//  if( time > timer.getTime() ) {
//    SDL_Delay( time - timer.getTime() );
//	}
  a->execute( time );
  erase( this->begin() );
  delete a;
}

void
ActionQueue::executeAllEvents() {
  while( size() > 0 ) {
    executeNextEvent();
  }
}

void ActionQueue::executeEventsBefore( Uint32 time ) {
	while( ( size() > 0 ) && ( timeOfNextEvent() < time ) ) {
 	  executeNextEvent();
	}
}

void
ActionQueue::executePastEvents() {
	executeEventsBefore( timer.getTime() );
}

