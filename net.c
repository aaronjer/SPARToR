
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include <stdlib.h>
#include "net.h"

UDPsocket hostsock = NULL;
UDPsocket clientsock = NULL;
IPaddress ipaddr;


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


