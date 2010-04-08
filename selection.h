// Copyright 2005 by Anthony Liekens anthony@liekens.net

#ifndef SELECTION_H
#define SELECTION_H

#include "config.h"
#include "coordinate.h"

#include "SDL.h"

class Selection {
private:
	Coordinate c1, c2;
	enum SelectionStates { NOT_SELECTING, SELECTING };
	SelectionStates state;
public:
	Selection();
	bool update();
	void render( SDL_Surface *screen );
	double getMinX() const;
	double getMaxX() const;
	double getMinY() const;
	double getMaxY() const;
};

#endif
