
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "net.h"
#include "console.h"


UDPsocket hostsock = NULL;
UDPsocket clientsock = NULL;
IPaddress ipaddr;
int maxclients = 32;


void disconnect(){
  if( hostsock ){
    //TODO: close lots of stuff
    SDLNet_UDP_Close(hostsock);
    hostsock = NULL;
    SJC_Write("Host stopped.");
  }else if( clientsock ){
    //TODO: maybe missing something
    SDLNet_UDP_Close(clientsock);
    clientsock = NULL;
    SJC_Write("Disconnected from host.");
  }else{
    SJC_Write("Nothing to disconnect from.");
  }
}


