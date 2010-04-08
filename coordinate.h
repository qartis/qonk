// Copyright 2005 by Anthony Liekens anthony@liekens.net

#ifndef COORDINATE_H
#define COORDINATE_H

class Coordinate {
private:
	double x, y;
public:
	Coordinate();
	Coordinate( double x, double y );
	
	void setX( double x );
	void setY( double y );
	void setXMapped( int x );
	void setYMapped( int y );
	
	double getX() const;
	double getY() const;
	int getXMapped() const;
	int getYMapped() const;
	
	double distance( const Coordinate& c ) const;
	
};

#endif
