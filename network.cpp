#include "network.h"
#include "config.h"

Server::Server(){

    if(SDLNet_ResolveHost(&ip,"localhost",9999)==-1) {
            printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
                exit(1);
    }

    tcpsock=SDLNet_TCP_Open(&ip);
    if(!tcpsock) {
            printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
                exit(2);
    }

}
