#include "network.h"
#include "config.h"
#include "universe.h"

#define MAXLEN 2048
static int port = 6667;

typedef enum netcmd {
    CMD_NAME,
    CMD_SAY,
    CMD_KILL,
    CMD_PING,
    CMD_PONG,
    CMD_STATE,
    CMD_ERR,
} netcmd;

typedef struct netmsg {
    netcmd cmd;
    char *arg;
} netmsg;

inline int streq(char *a, char *b){
    return strcmp(a,b) == 0;
}

netmsg parsecmd(char *cmd){
    netmsg m;
    char *space = strchr(cmd, ' ');
    m.arg = NULL;
    if (space != NULL){
        *space = '\0';
        m.arg = space + 1;
    }

    if (streq(cmd,"name")){
        m.cmd = CMD_NAME;
    } else if (streq(cmd,"say")){
        m.cmd = CMD_SAY;
    } else if (streq(cmd,"kill")){
        m.cmd = CMD_KILL;
    } else if (streq(cmd,"ping")){
        m.cmd = CMD_PING;
    } else if (streq(cmd,"pong")){
        m.cmd = CMD_PONG;
    } else if (streq(cmd,"state")){
        m.cmd = CMD_STATE;
    } else {
        m.cmd = CMD_ERR;
    }
    return m;
}

int readLine(TCPsocket sock, char *buf, int max){
    int i = 0;
    while(i < max){
        if (SDLNet_TCP_Recv(sock,&(buf[i]),1) != 1){
            return -1;
        }
        if (buf[i] == '\n'){
            buf[i] = '\0';
            return i;
        }
        i++;
    }
    return i;
}

void Server::send(const char *fmt, ...){
    if (sock == NULL){
        return;
    }
    char buf[MAXLEN];
    va_list ap;

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    SDLNet_TCP_Send(sock, buf, strlen(buf));
    printf("sent '%s'\n",buf);
}

void Server::connect(Universe *_u){
    u = _u;
    SDLNet_Init();
    SDLNet_ResolveHost(&ip,server,port);
    sock = SDLNet_TCP_Open(&ip);
    if (!sock){
        printf("could not connect to %s:%d\n",server,port);
        return;
    }
    set = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(set,sock);
    send("name %s\n",myname);
    last_ping_time = SDL_GetTicks();
}

void Server::process(){
    if (sock == NULL) return;

    Uint32 now = SDL_GetTicks();
    if (now - last_ping_time > 1000){
        send("ping %d\n",now);
        last_ping_time = now;
    }

    int n = SDLNet_CheckSockets(set, 0);
    if (n < 1 || SDLNet_SocketReady(sock) == 0){
        return;
    }

    char buf[MAXLEN];
    if (readLine(sock,buf,MAXLEN) < 1){
        printf("server died?\n");
        exit(1);
    }
    netmsg m = parsecmd(buf);
    switch(m.cmd){
    case CMD_SAY:
        u->messages->addMessage(Message(m.arg, 20000, 0xffffff ) );
        break;
    case CMD_KILL:
        if (streq(m.arg,myname)){
            printf("Server killed me\n");
            exit(1);
        }
        break;
    case CMD_PING:
        send("pong %s\n",m.arg);
        break;
    case CMD_PONG:
        lag = ((float)(now-atoi(m.arg))) / 2.0f / 10.0f;
        break;
    case CMD_STATE:
        
    default:
        printf("unimplemented command: %d '%s'\n",m.cmd,m.arg);
        break;
    }
}

User::User(TCPsocket _sock){
    sock = _sock;
    name = NULL;
}

void Users::connect(Universe *_u){
    u = _u;
    SDLNet_Init();
    SDLNet_ResolveHost(&ip,NULL,port);
    sock = SDLNet_TCP_Open(&ip);
    set = SDLNet_AllocSocketSet(10);
    SDLNet_TCP_AddSocket(set,sock);
}

void Users::handle_command(char *buf, User *u){
    netmsg m = parsecmd(buf);
    switch(m.cmd){
    case CMD_NAME:
        if (u->name != NULL){
            free(u->name);
        }
        u->name = strdup(m.arg);
        break;
    case CMD_SAY:
        char buf[MAXLEN];
        sprintf(buf,"%s: %s",u->name,m.arg);
        this->u->messages->addMessage(Message(buf, 20000, 0xffffff ) );
        send("say %s\n",buf);
        break;
    case CMD_KILL:
        //ignore
        break;
    case CMD_PING:
        send(u,"pong %s\n",m.arg);
        break;
    default:
        printf("unknown network command from user %s: %d '%s'\n",u->name,m.cmd,m.arg);
    }
}

void Users::process(){
    int n = SDLNet_CheckSockets(set, 0);
    if (n < 1) return;

    if (SDLNet_SocketReady(sock)){
        printf("new user...\n");
        TCPsocket newuser = SDLNet_TCP_Accept(sock);

        printf("Accepted...\n");
        User *u = new User(newuser);
        push_back(u);
        SDLNet_TCP_AddSocket(set,newuser);
    }

    User *u;
    for( iterator i = begin(); i != end(); i++ ){
        u = *i;
        if (SDLNet_SocketReady(u->sock)){
            int len;
            char buf[MAXLEN];
            if ((len = readLine(u->sock,buf,sizeof(buf))) > 0){
                fprintf(stderr,"got len=%d text from '%s': '%s'\n",len,u->name,buf);
                handle_command(buf,u);
            } else {
                printf("dead connection\n");
                SDLNet_TCP_DelSocket(set, u->sock);
                SDLNet_TCP_Close(u->sock);
                i = erase(i);
            }
        }
    }
}

inline bool Users::sendToUser(User *u, const char *buf){
    int len = strlen(buf);
    if (SDLNet_TCP_Send(u->sock, buf, len) < len){
        printf("dead connection\n");
        SDLNet_TCP_DelSocket(set, u->sock);
        SDLNet_TCP_Close(u->sock);
        return false;
    }
    return true;
}

void Users::send(User *u, const char *fmt, ...){
    char buf[MAXLEN];
    va_list ap;

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    sendToUser(u,buf);
    printf("sent '%s' to user '%s'\n",buf,u->name);
}

void Users::send(const char *fmt, ...){
    char buf[MAXLEN];
    va_list ap;

    va_start(ap, fmt);
    vsprintf(buf, fmt, ap);
    va_end(ap);

    User *u;
    for( iterator i = begin(); i != end(); i++ ){
        u = *i;
        if (sendToUser(u,buf) == false){
            i = erase(i);
        }
    }
    printf("sent '%s' to all clients\n",buf);
}
