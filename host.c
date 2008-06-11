
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "main.h"
#include "console.h"
#include "command.h"
#include "host.h"
#include "net.h"




static UDPpacket *pkt;
static CLIENT_t clients[32];


void host_start(int port){
  if(!port) port=HOSTPORT;
  if( hostsock ){ SJC_Write("Already running as a host. Type disconnect to stop."); return; }
  if( clientsock ){ SJC_Write("Already connected to a host. Type disconnect if that ain't cool."); return; }
  if( !(hostsock = SDLNet_UDP_Open(port)) ){ SJC_Write("Error: Could not open host socket!"); SJC_Write(SDL_GetError()); return; }
  pkt = SDLNet_AllocPacket(1000);
  SJC_Write("Host started on port %d.",port);
}


void host_stop()
{
  SDLNet_FreePacket(pkt);
}


void host(){
  char s[1000];
  int status;

  //look for new connections
  status = SDLNet_UDP_Recv(hostsock,pkt);
  switch( status ){
    case -1:
      SJC_Write("Network Error: Failed to check for new connections.");
      SJC_Write(SDL_GetError());
      break;
    case 1:
      SJC_Write("Received packet...");
      sprintf(s,"channel=%d | maxlen=%d | len=%d | ipv4=%d | port=%d",pkt->channel,pkt->maxlen,pkt->len,pkt->address.host,pkt->address.port);
      SJC_Write(s);
      strncpy(s,pkt->data,pkt->len);
      s[pkt->len]='\0';
      SJC_Write(s);
      break;
  }
}



