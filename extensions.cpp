// Copyright 2005 by Anthony Liekens anthony@liekens.net

#include "extensions.h"
#include "SDL_gfxPrimitives.h"

void 
alphaBlend( SDL_Surface* surface, Uint8 alpha ) {
	if( alpha == 0 ) {
		for( int x = 0; x < surface->w; x++ )
		for( int y = 0; y < surface->h; y++ ) {
			Uint32 *bufp;
			bufp = (Uint32 *)surface->pixels + y*surface->pitch/4 + x;
			if( *bufp != 0 )
				*bufp &= 0x00ffffff;
		}
	} if( alpha != 255 ) {
		for( int x = 0; x < surface->w; x++ )
		for( int y = 0; y < surface->h; y++ ) {
			Uint32 *bufp;
			bufp = (Uint32 *)surface->pixels + y*surface->pitch/4 + x;
			if( *bufp != 0 ) {
				// seperate RGB from A, update A for new value, OR them together again
				Uint32 color = ( *bufp & 0x00ffffff ) | ( ( *bufp >> 24 ) * alpha >> 8 ) << 24;
				*bufp = color;
			}
		}
	}
}

void
drawSelector( SDL_Surface *screen, Sint16 x1, Sint16 y1, Sint16 width, Sint16 height, Uint8 R, Uint8 G, Uint8 B ) {
	hlineRGBA( screen, x1, x1 + 2, y1, R, G, B, 255 );
	hlineRGBA( screen, x1 + width - 2, x1 + width, y1, R, G, B, 255 );
	hlineRGBA( screen, x1, x1 + 2, y1 + height, R, G, B, 255 );
	hlineRGBA( screen, x1 + width - 2, x1 + width, y1 + height, R, G, B, 255 );
	vlineRGBA( screen, x1, y1, y1 + 2, R, G, B, 255 );
	vlineRGBA( screen, x1, y1 + height, y1 + height - 2, R, G, B, 255 );
	vlineRGBA( screen, x1 + width, y1, y1 + 2, R, G, B, 255 );
	vlineRGBA( screen, x1 + width, y1 + height, y1 + height - 2, R, G, B, 255 );
}

void
drawPlanet( SDL_Surface* screen, Sint16 x, Sint16 y, int size, Uint8 R, Uint8 G, Uint8 B ) {
	filledCircleRGBA( screen, x, y, size + 2, 0, 0, 0, 128 );
	filledCircleRGBA( screen, x, y, size, R, G, B, 255 );
	aacircleRGBA( screen, x, y, size, R, G, B, 255 );
	filledEllipseRGBA( screen, x, y - size / 2, size, size / 2, 255 - 4 * ( 255 - R ) / 5, 255 - 4 * ( 255 - G ) / 5, 255 - 4 * ( 255 - B ) / 5, 255 );
	aaellipseRGBA( screen, x, y - size / 2, size, size / 2, 255 - 4 * ( 255 - R ) / 5, 255 - 4 * ( 255 - G ) / 5, 255 - 4 * ( 255 - B ) / 5, 255 );
}

void 
drawPlanet( SDL_Surface* screen, Sint16 x, Sint16 y, int size, Uint32 color ) {
	drawPlanet( screen, x, y, size, getRed( color ), getGreen( color ), getBlue( color ) );
}

double
frand() {
	return (double)rand() / RAND_MAX;
}

double
frand( double max ) {
	return max * frand();
}

Uint32 HSVtoRGB( float h, float s, float v )
{
	int i;
	float f, p, q, t, r, g, b;

	if( s == 0 ) {

		// achromatic (grey)
		r = g = b = v;

	} else {

		h /= 60;			// sector 0 to 5
		i = (int)h;
		f = h - i;			// factorial part of h
		p = v * ( 1 - s );
		q = v * ( 1 - s * f );
		t = v * ( 1 - s * ( 1 - f ) );
	
		switch( i ) {
			case 0:
				r = v;
				g = t;
				b = p;
				break;
			case 1:
				r = q;
				g = v;
				b = p;
				break;
			case 2:
				r = p;
				g = v;
				b = t;
				break;
			case 3:
				r = p;
				g = q;
				b = v;
				break;
			case 4:
				r = t;
				g = p;
				b = v;
				break;
			default:		// case 5:
				r = v;
				g = p;
				b = q;
			break;
		}

	}

	return 256 * 256 * (int)( 255 * r ) + 256 * (int)( 255 * g ) + (int)( 255 * b );

}

Uint8 getRed( Uint32 color ) {
	return ( color & 0xFF0000 ) >> 16;
}

Uint8 getGreen( Uint32 color ) {
	return ( color & 0x00FF00 ) >> 8;
}

Uint8 getBlue( Uint32 color ) {
	return color & 0x0000FF;
}

