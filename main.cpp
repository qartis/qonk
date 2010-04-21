// Copyright 2005 by Anthony Liekens anthony@liekens.net

#include "config.h"
#include "coordinate.h"
#include "stars.h"
#include "planets.h"
#include "selection.h"
#include "players.h"
#include "universe.h"
#include "fonts.h"
#include "network.h"

#include "SDL_gfxPrimitives.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include <math.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <list>
using namespace std;

Font* font;
Uint32 FPStimer, FPScounter;
double FPS;
bool is_server;
Network *net;
char myname[64];
Uint32 mycolour = 0x00FFFF;
char *server = "localhost";
float lag = 0;
SDL_Surface *screen;
int flags = SDL_SWSURFACE;

enum States { LOBBY, PLAYING, LOST, WON };
States state;

void send_state(Universe *u, Players *p){
    char buf[2048];
    sprintf(buf,"state ");

    for(list<Planet>::iterator p = u->planets->begin(); p != u->planets->end(); p++ ) {
        sprintf(buf+strlen(buf),"p %d %d %f %d %d %d ",p->rotationSpeed,p->rotationOffset,p->rotationDistance,p->shipCreationSpeed,p->size,p->isAMoon);
    }

    net->send(buf);
}

void render( SDL_Surface* screen, Universe* universe, Players* players, char *typing_buffer, bool typing_mode) {
	boxRGBA( screen, 0, 0, settings.getScreenWidth(), settings.getScreenHeight(), 0, 0, 0, 255 );
	universe->renderBackground( screen );
	players->render( screen );
	universe->renderForeground( screen );
	stringstream s;
	s << timer.getTimeMMSSHH();
	FPScounter++;
	if( SDL_GetTicks() - FPStimer >= 750 ) {
        FPS = ( FPS + 1000.0 * FPScounter / ( SDL_GetTicks() - FPStimer ) ) / 2;
		FPStimer = SDL_GetTicks();
		FPScounter = 0;
	}
	s << " (" << setiosflags(ios::fixed) << setprecision(2) << FPS << "FPS) ";
    if (is_server){
        font->render( screen, settings.getGameWidth() - 20, 5, "server", 0xff, 0xff, 0xff);
    } else {
        s << setprecision(1) << lag << " ms";
    }
	font->render( screen, 13, settings.getGameHeight() - 30, s.str().c_str(), 0x80, 0x80, 0x80 );
    if (typing_mode == true){
        int len = strlen(typing_buffer);
        if ((SDL_GetTicks() % 1000) > 500) {
            strcat(typing_buffer,"_");
        }
        font->render( screen, 13, 30, typing_buffer, 0x80, 0x80, 0x80);
        typing_buffer[len] = '\0';
    }

	SDL_UpdateRect(screen, 0, 0, settings.getScreenWidth(), settings.getScreenHeight() );
  
}

void setvidmode(){
	ASSERT(screen = SDL_SetVideoMode( settings.getScreenWidth(), settings.getScreenHeight(), 32, flags));
}

int main(int argc, char** argv) {
    FILE *f = fopen("name.txt","r");
    if (f != NULL){
        fscanf(f,"%s",myname);
        fclose(f);
    }
    if (strlen(myname) < 1){
        strcpy(myname,"noname");
    }

    if (argc > 1){
        server = argv[1];
    }

	state = PLAYING;

	int numberOfPlanets = 6;
	int numberOfComputerPlayers = 1;

	if( argc == 3 ) {
		numberOfPlanets = atoi( argv[ 1 ] );
		numberOfComputerPlayers = atoi( argv[ 2 ] );
	}
	if( numberOfPlanets <= 1 ) numberOfPlanets = 2;
	if( numberOfComputerPlayers > numberOfPlanets - 1 ) numberOfComputerPlayers = numberOfPlanets - 1;
	if( numberOfComputerPlayers <= 1 ) numberOfComputerPlayers = 1;

    if (strstr(argv[0],"server") != NULL){
        is_server = 1;
        net = new Users;
    } else {
        is_server = 0;
        net = new Server;
    }

	
	/* Init SDL */
	ASSERT(SDL_Init( SDL_INIT_TIMER | SDL_INIT_VIDEO ) > -1);
	timer.start();
	atexit(SDL_Quit);
	ASSERT(TTF_Init() > -1);
	atexit(TTF_Quit);
	font = new Font("font.ttf", 18);
	
	srand(time(NULL));
	
	SDL_WM_SetCaption("Qonk", "Qonk");

    setvidmode();
	
	Universe* universe = new Universe(numberOfPlanets);
	
	Players* players = new Players(universe);
	universe->actionQueue->scheduleAction(1000, new UpdatePlayersStatsAction(players));

	Planets::iterator planetIterator = universe->planets->begin();
	HumanPlayer* humanPlayer = NULL;
    if (!is_server){
        humanPlayer = new HumanPlayer(universe, &(*planetIterator), 3, 0xffffff);
        players->push_back( humanPlayer );
        planetIterator++;
    }
	
	int hueCounter = rand() % 360;
	int counter = 0;
	do {
		if( counter < numberOfComputerPlayers ) {
			players->push_back(new ComputerPlayer(universe, &(*planetIterator), 3, HSVtoRGB(hueCounter, 0.9, 0.9)));
			counter++;
			hueCounter = ( hueCounter + 360 / numberOfComputerPlayers ) % 360;
		} else {
			if(!planetIterator->getMoon()){
				players->push_back( new NeutralPlayer( universe, &(*planetIterator), 1 + rand() % 3, 0x606060 ) );
			} else {
				players->push_back( new NeutralPlayer( universe, &(*planetIterator), 1 + rand() % 2, 0x606060 ) );
            }
        }
        planetIterator++;
	} while(planetIterator != universe->planets->end());

	Selection selection;
	
	bool showEnemyShips = false;
	bool quit = false;
    int  typing_pos = 0;
    char typing_buffer[1024];
    bool typing_mode = false;
    typing_buffer[0] = '\0';

    net->connect(universe);

    SDL_EnableUNICODE(1);
    SDL_Event event;

	while(!quit){
        net->process();

		if( !timer.isPaused() ) {
			universe->update();
			players->update();
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
            case SDL_KEYDOWN:
                if (typing_mode == false){
                    switch (event.key.keysym.sym){
                    case SDLK_z:
                        send_state(universe, players);
                        break;
                    case SDLK_s:
                        SDL_SaveBMP( SDL_GetVideoSurface(), "screenshot.bmp" );
                        universe->messages->addMessage( timer.getTime(), Message("saved screenshot to screenshot.bmp", 20000, 0xffffff ) );
                        break;
                    case SDLK_p:
                        timer.pause();
                        break;
                    case SDLK_f:
                        flags ^= SDL_FULLSCREEN;
                        setvidmode();
                        break;
                    case SDLK_q:
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_EQUALS:
                        //if (event.key.keysym.mod & KMOD_LSHIFT || event.key.keysym.mod & KMOD_RSHIFT){ }
                        break;
                    case SDLK_RETURN:
                        typing_mode = true;
                        typing_pos = 0;
                        break;
                    default:
                        break;
                    }
                } else {
                    if (event.key.keysym.sym == SDLK_RETURN){
                        typing_mode = false;
                        typing_buffer[typing_pos] = '\0';
                        printf("sending message to server: '%s'\n",typing_buffer);
                        if (is_server){
                            net->send("%s\n",typing_buffer);
                        } else {
                            net->send("say %s\n",typing_buffer);
                        }
                        typing_buffer[0] = '\0';
                    } else if (isprint(event.key.keysym.sym)){
                        typing_buffer[typing_pos++] = event.key.keysym.unicode & 0x7F;
                        typing_buffer[typing_pos] = '\0';
                    } else if (event.key.keysym.sym == SDLK_BACKSPACE){
                        typing_pos--;
                        typing_buffer[typing_pos] = '\0';
                    } else if (event.key.keysym.sym == SDLK_ESCAPE){
                        typing_mode = false;
                        typing_buffer[0] = '\0';
                    }
                }
                break;
            case SDL_QUIT:
                quit = true;
                break;
			}
			
		}
		
		if( humanPlayer && humanPlayer->getPoints() == 0 ) {
			showEnemyShips = true;
			state = LOST;
		}
		
		if( state == PLAYING ) {
			bool allPlayersDead = true;
			for( Players::iterator i = players->begin(); i != players->end(); i++ ) {
				if( (*i)->getPlayerType() == Player::COMPUTER ){
					if( ((ComputerPlayer*)(*i))->getPoints() > 0 ){
						allPlayersDead = false;
                        break;
                    }
                }
			}
			if( allPlayersDead == true ) {
				universe->messages->addMessage( timer.getTime() + 200, Message( "You have QONKuered the solar system! You won!", 20000, 0xffffff ) );
                universe->messages->addMessage( timer.getTime() + 600, Message( "Press [Escape] ...", 40000, 0x808080 ) );
				state = WON;
			}
		}
		
		Uint8 *keystate = SDL_GetKeyState(NULL);
		bool show = false;
		if( keystate[SDLK_e] ) {
			show = true;
		}
		if( keystate[SDLK_a] && humanPlayer){
			humanPlayer->selectAllShips();
		}
		
		for( Players::iterator i = players->begin(); i != players->end(); i++ ) {
			if( (*i)->getPlayerType() == Player::COMPUTER )
				((ComputerPlayer*)(*i))->setDisplayShips( show || showEnemyShips || is_server);
			if( (*i)->getPlayerType() == Player::NEUTRAL )
				((NeutralPlayer*)(*i))->setDisplayShips( show || showEnemyShips || is_server);
		}
		
		// Keep shut for some time, not to use too much CPU
		
		render(screen, universe, players, typing_buffer, typing_mode);
		SDL_Delay( 20 );
		
	}
	
	delete font;
	delete universe;
	delete players;
	
	return 0;
	
}
