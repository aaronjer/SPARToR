
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "main.h"
#include "console.h"
#include "command.h"
#include "net.h"


void command(const char *s){
  UDPpacket *pkt;
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
      if( hostsock ){ SJC_Write("Already running as a host. Type disconnect to stop."); break; }
      if( clientsock ){ SJC_Write("Already connected to a host. Type disconnect if that ain't cool."); break; }
      if( !(hostsock = SDLNet_UDP_Open(31103)) ){ SJC_Write("Error: Could not open host socket!"); SJC_Write(SDL_GetError()); break; }
      SJC_Write("Host started.");
    }else if( strcmp(q,"connect")==0 ){
      if( hostsock ){ SJC_Write("Already running as a host. Type disconnect to stop."); break; }
      if( clientsock ){ SJC_Write("Already connected to a host. Type disconnect if that ain't cool."); break; }
      q = strtok(NULL," :");
      if( !q ){ SJC_Write("Error: Please specify host."); break; }
      SDLNet_ResolveHost(&ipaddr,q,31103);
      if( ipaddr.host==INADDR_NONE ){ SJC_Write("Error: Could not resolve host!"); break; }
      if( !(clientsock = SDLNet_UDP_Open(31109)) ){ SJC_Write("Error: Could not open client socket!"); SJC_Write(SDL_GetError()); break; }
      SJC_Write("Connecting...");
      pkt = SDLNet_AllocPacket(1000);
      pkt->address = ipaddr;
      strcpy(pkt->data,"SPARToR Protocol v0.9a");
      pkt->len = strlen(pkt->data);
      if( !SDLNet_UDP_Send(clientsock,-1,pkt) ){
        SJC_Write("Error: Could not send connect packet!");
        SJC_Write(SDL_GetError());
        SDLNet_UDP_Close(clientsock);
        clientsock = NULL;
        break;
      }
      SDLNet_FreePacket(pkt);
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



