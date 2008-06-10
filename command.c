
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "main.h"
#include "console.h"
#include "command.h"


void command(const char *s){
  char *p, *q;
  p = malloc(strlen(s)+1);
  strcpy(p,s);
  q = strtok(p," ");
  TRY
    if( q==NULL )
      ;
    else if( strcmp(q,"exit")==0 || strcmp(q,"quit")==0 )
      cleanup();
    else if( strcmp(q,"listen")==0 ){
      if( hostsock ){
        SJC_Write("Already running as a host. Type disconnect to stop.");
        break;
      }
      if( !(hostsock = SDLNet_UDP_Open(31103)) ){
        SJC_Write("Error: Could not open host socket!");
        SJC_Write(SDL_GetError());
        break;
      }
      SJC_Write("Host started.");
    }else if( strcmp(q,"connect")==0 ){
      if( clientsock ){
        SJC_Write("Already connected to a host. Type disconnect if that ain't cool.");
        break;
      }
      if( !(clientsock = SDLNet_UDP_Open(31109)) ){
        SJC_Write("Error: Could not open client socket!");
        SJC_Write(SDL_GetError());
        break;
      }
      SJC_Write("Connecting...");
    }else if( strcmp(q,"disconnect")==0 ){
      if( hostsock ){
        SDLNet_UDP_Close(hostsock);
        hostsock = NULL;
        SJC_Write("Host stopped.");
      }else if( clientsock ){
        SDLNet_UDP_Close(clientsock);
        clientsock = NULL;
        SJC_Write("Disconnected from host.");
      }else
        SJC_Write("Nothing to disconnect from.");
    }else
      SJC_Write("Huh?");
  HARDER
  free(p);
}



