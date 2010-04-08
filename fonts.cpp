// Copyright 2005 by Anthony Liekens anthony@liekens.net

#include "fonts.h"

#include "extensions.h"

Font::Font(const char *fileName, int size ) {
  font = TTF_OpenFont( fileName, size );
	if ( font == NULL ) {
		cerr << "Couldn't load " << size << "pt font from " << fileName << ": " << SDL_GetError() << endl;
		exit( 1 );
	}
	TTF_SetFontStyle( font, TTF_STYLE_NORMAL );
}

void
Font::render( SDL_Surface* screen, int x, int y, const char* text, Uint8 r, Uint8 g, Uint8 b, Uint8 a ) {
  SDL_Color color = { r, g, b, a };
  SDL_Surface* textSurface = TTF_RenderText_Blended( font, text, color );
	if ( textSurface != NULL ) {
		SDL_Rect dstrect;
		dstrect.x = x;
		dstrect.y = y;
		dstrect.w = textSurface->w;
		dstrect.h = textSurface->h;
		alphaBlend( textSurface, a );
		SDL_BlitSurface( textSurface, NULL, screen, &dstrect );
		SDL_FreeSurface( textSurface );
	}
}

void Font::renderCenterJustified( SDL_Surface* screen, int x, int y, const char* text, Uint8 r, Uint8 g, Uint8 b, Uint8 a ) {
    SDL_Color color = { r, g, b, 0 };
    SDL_Surface* textSurface = TTF_RenderText_Blended( font, text, color );
    if ( textSurface == NULL ) {
        return;
    }
    SDL_Rect dstrect;
    dstrect.x = x - textSurface->w / 2;
    dstrect.y = y - textSurface->h / 2;
    dstrect.w = textSurface->w;
    dstrect.h = textSurface->h;
    alphaBlend( textSurface, a );
    SDL_BlitSurface( textSurface, NULL, screen, &dstrect );
    SDL_FreeSurface( textSurface );
}

void Font::renderRightJustified( SDL_Surface* screen, int x, int y, const char* text, Uint8 r, Uint8 g, Uint8 b, Uint8 a ) {
    SDL_Color color = { r, g, b, 0 };
    SDL_Surface* textSurface = TTF_RenderText_Blended( font, text, color );
    if ( textSurface == NULL ) {
        return;
    }
    SDL_Rect dstrect;
    dstrect.x = x - textSurface->w;
    dstrect.y = y;
    dstrect.w = textSurface->w;
    dstrect.h = textSurface->h;
    alphaBlend( textSurface, a );
    SDL_BlitSurface( textSurface, NULL, screen, &dstrect );
    SDL_FreeSurface( textSurface );
}

int Font::getHeight() {
	return TTF_FontHeight( font );
}
