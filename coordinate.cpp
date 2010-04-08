// Copyright 2005 by Anthony Liekens anthony@liekens.net

#include "coordinate.h"

#include "config.h"
#include <math.h>

Coordinate::Coordinate() {
	x = 0;
	y = 0;
}

Coordinate::Coordinate( double x, double y ) {
	this->x = x;
	this->y = y;
}

void
Coordinate::setX( double x ) {
	this->x = x;
}

void
Coordinate::setY( double y ) {
	this->y = y;
}

void
Coordinate::setXMapped( int x ) {
	this->x = ( (double)x - settings.getGameOffsetX() ) / settings.getGameWidth();
}

void
Coordinate::setYMapped( int y ) {
	this->y = (double)y / settings.getGameHeight();
}

double
Coordinate::getX() const {
	return x;
}

double
Coordinate::getY() const {
	return y;
}

int
Coordinate::getXMapped() const {
	return settings.getGameOffsetX() + ( int )( settings.getGameWidth() * x );
}

int
Coordinate::getYMapped() const {
	return ( int )( settings.getGameHeight() * y );
}

double
Coordinate::distance( const Coordinate& c ) const {
	double dx = c.getX() - getX();
	double dy = c.getY() - getY();
	return sqrt( dx * dx + dy * dy );
}

