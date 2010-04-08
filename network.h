#include <SDL.h>
#include <SDL_net.h>
#include <vector>
#include <stdlib.h>

class Server {
private:
    IPaddress ip;
    TCPsocket tcpsock;

public:
	Server();
    void connect();
};

