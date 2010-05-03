//#include <iostream>
//#include <iomanip>
//#include <sstream>
#include "SDL.h"
#include "universe.h"
#include "network.h"
#include "config.h"
#include "game.h"

using namespace std;

void dump(uint8_t *p, uint16_t len){
    int i=0,j;
    uint8_t c;

    if (len<16){
        goto print;
    }

    for (;i<len/16;i++){
print:
        fprintf(stderr,"%04x  ",i*16);
        for (j=0;j<16;j++){
            if (i*16+j < len){
                fprintf(stderr,"%02x ",p[i*16+j]);
            } else {
                fprintf(stderr,"   ");
            }
        }
        fprintf(stderr,"  ");
        for (j=0;j<16;j++){
            if (i*16+j < len){
                c = p[i*16+j];
            } else {
                c = ' ';
            }
            fprintf(stderr,"%c", isprint(c) ? c : '.');
        }
        fprintf(stderr,"\n");
    }
    fprintf(stderr,"\n");
}

extern Network *net;
extern Settings settings;

typedef struct utf8 {
    char data[4];
} utf8;

utf8 utf8_len(uint16_t wc){
    utf8 u;
    char *buf = &(u.data[0]);
    if (wc < 0x80){
        buf[0] = wc;
        buf[1] = 0x00;
    } else if (wc < 0x800) {
        buf[0] = (0xC0 | wc>>6);
        buf[1] = (0x80 | wc & 0x3F);
        buf[2] = 0x00;
    } else {
        buf[0] = (0xE0 | wc>>12);
        buf[1] = (0x80 | wc>>6 & 0x3F);
        buf[2] = (0x80 | wc & 0x3F);
        buf[3] = 0x00;
    }

    return u;
}
utf8 uni_to_utf8(uint16_t wc){
    utf8 u;
    char *buf = &(u.data[0]);
    if (wc < 0x80){
        buf[0] = wc;
        buf[1] = 0x00;
    } else if (wc < 0x800) {
        buf[0] = (0xC0 | wc>>6);
        buf[1] = (0x80 | wc & 0x3F);
        buf[2] = 0x00;
    } else {
        buf[0] = (0xE0 | wc>>12);
        buf[1] = (0x80 | wc>>6 & 0x3F);
        buf[2] = (0x80 | wc & 0x3F);
        buf[3] = 0x00;
    }

    return u;
}


void send_state(Universe *u, Players *p){
    char buf[2048];
    sprintf(buf,"state ");

    for(vector<Planet>::iterator p = u->planets->begin(); p != u->planets->end(); p++ ) {
        sprintf(buf+strlen(buf),"p %d %d %f %d %d %d ",p->rotationSpeed,p->rotationOffset,p->rotationDistance,p->shipCreationSpeed,p->size,p->isAMoon);
    }

    net->send(buf);
}

void rendergame(Players* players, char *typing_buffer, bool typing_mode) {
	boxRGBA( screen, 0, 0, settings.getScreenWidth(), settings.getScreenHeight(), 0, 0, 0, 255 );
	universe->renderBackground( screen );
	players->render( screen );
	universe->renderForeground( screen );
    renderstats();
    if (typing_mode == true){
        int len = strlen(typing_buffer);
        if ((SDL_GetTicks() % 1000) > 500) {
            strcat(typing_buffer,"_");
        }
        font->render( screen, 13, 30, typing_buffer, 0xff, 0xff, 0xff);
        typing_buffer[len] = '\0';
    }

	SDL_UpdateRect(screen, 0, 0, settings.getScreenWidth(), settings.getScreenHeight() );
}

States game(int numberOfPlanets, int numberOfComputerPlayers){
    enum States state = PLAYING;
	universe = new Universe(numberOfPlanets);
	
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
    #define msg_buf_len 1024
    char typing_buffer[msg_buf_len];
    bool typing_mode = false;
    typing_buffer[0] = '\0';

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
                        //send_state(universe, players);
                        break;
                    case SDLK_s:
                        SDL_SaveBMP(SDL_GetVideoSurface(), "screenshot.bmp");
                        universe->messages->addMessage( timer.getTime(), Message("saved screenshot to screenshot.bmp", 20000, 0xffffff ) );
                        break;
                    case SDLK_p:
                        timer.pause();
                        break;
                    case SDLK_f:
                        settings.setvidmode(SDL_FULLSCREEN);
                        break;
                    case SDLK_q:
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_EQUALS:
                        //if (event.key.keysym.mod & KMOD_SHIFT){ }
                        break;
                    case SDLK_RETURN:
                        typing_mode = true;
                        typing_pos = 0;
                        break;
                    default:
                        break;
                    }
                } else {
                    if (event.key.keysym.sym == SDLK_RETURN || typing_pos >= (msg_buf_len - 1)){
                        typing_mode = false;
                        typing_buffer[typing_pos] = '\0';
                        printf("sending message to server: '%s'\n",typing_buffer);
                        if (is_server){
                            net->send("%s\n",typing_buffer);
                        } else {
                            net->send("say %s\n",typing_buffer);
                        }
                        typing_buffer[0] = '\0';
                    } else if (event.key.keysym.sym < 255 && isprint(event.key.keysym.sym)){
                        typing_buffer[typing_pos++] = event.key.keysym.unicode & 0x7F;
                        typing_buffer[typing_pos] = '\0';
                    } else if (event.key.keysym.sym == SDLK_BACKSPACE){
                        if (typing_pos < 1){
                            break;
                        }
                        typing_pos--;
                        if (typing_buffer[typing_pos] & 0x80){
                            do {
                                typing_pos--;
                            } while ((typing_buffer[typing_pos] & 0xF0) != 0xF0 && (typing_buffer[typing_pos] & 0xF0) != 0xE0 && typing_pos > -1);
                        }
                        typing_buffer[typing_pos] = '\0';
                    } else if (event.key.keysym.sym == SDLK_ESCAPE){
                        typing_mode = false;
                        typing_buffer[0] = '\0';
                    } else {
                        utf8 a = uni_to_utf8(event.key.keysym.unicode);
                        strcat(typing_buffer,a.data);
                        typing_pos += strlen(a.data);
                    }
                    rendergame(players, typing_buffer, typing_mode);
                }
                break;
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_VIDEORESIZE:
                settings.setResolution(event.resize.w,event.resize.h);
                rendergame(players, typing_buffer, typing_mode);

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
		
		rendergame(players, typing_buffer, typing_mode);
		SDL_Delay( 20 );
	}
	
	delete universe;
    universe = NULL;
	delete players;
    players = NULL;

    return state;
}

