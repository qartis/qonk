#include <iostream>
#include <iomanip>
#include <sstream>
#include "SDL.h"
#include "universe.h"
#include "network.h"
#include "config.h"

using namespace std;

void renderlobby() {
	boxRGBA(screen, 0, 0, settings.getScreenWidth(), settings.getScreenHeight(), 0, 0, 0, 255 );
    renderstats();
    const char *name = "lobby";
    int height = font->getHeight();
    int width = font->getWidth(name);
    font->render(screen, (settings.getScreenWidth() - width) / 2, (settings.getScreenHeight() - height) / 2, name, 0xff, 0xff, 0xff);
	SDL_UpdateRect(screen, 0, 0, settings.getScreenWidth(), settings.getScreenHeight() );
}

void lobby(){
    SDL_Event event;
    int quit = 0;

	while(!quit){
        net->process();

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym){
                case SDLK_f:
                    settings.setvidmode(SDL_FULLSCREEN);
                    break;
                case SDLK_q:
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                default:
                    break;
                }
                break;
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_VIDEORESIZE:
                settings.setResolution(event.resize.w,event.resize.h);
                break;
			}
		}
        renderlobby();
    }
}
