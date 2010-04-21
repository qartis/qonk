#include "SDL.h"
#include "SDL_net.h"
#include <vector>
#include <list>
#include <stdlib.h>
#include <stdarg.h>

using namespace std;
class Universe;
extern char myname[64];
extern char *server;
extern float lag;

class Network {
public:
    virtual void process() = 0;
    virtual void connect(Universe*) = 0;
    virtual void send(const char *fmt, ...) = 0;
};

class Server: public Network {
public:
    IPaddress ip;
    TCPsocket sock;
    SDLNet_SocketSet set;
    Universe *u;
    Uint32 last_ping_time;
	void connect(Universe *);
    void process();
    void send(const char *fmt, ...);
};

class User {
public:
    User(TCPsocket _sock);
    TCPsocket sock;
    char *name;
};

class Users : public list<User*> , public Network {
public:
    Universe *u;
    IPaddress ip;
    TCPsocket sock;
    SDLNet_SocketSet set;
    void process();
    void connect(Universe*);
    void send(const char *fmt, ...);
    void send(User *u, const char *fmt, ...);
    bool sendToUser(User *u, const char *buf);
    void handle_command(char*,User *u);
};
