

#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "main.h"
#include "console.h"
#include "command.h"
#include "host.h"
#include "net.h"


static UDPpacket *pkt;
static CLIENT_t *clients;


void host_start(int port){
  if(!port) port=HOSTPORT;
  if( hostsock ){ SJC_Write("Already running as a host. Type disconnect to stop."); return; }
  if( clientsock ){ SJC_Write("Already connected to a host. Type disconnect if that ain't cool."); return; }
  if( !(hostsock = SDLNet_UDP_Open(port)) ){ SJC_Write("Error: Could not open host socket!"); SJC_Write(SDL_GetError()); return; }
  pkt = SDLNet_AllocPacket(1000);
  clients = calloc(maxclients,sizeof(CLIENT_t));
  SJC_Write("Host started on port %d.",port);
}


void host_stop(){
  SDLNet_FreePacket(pkt);
  free(clients);
}


void host(){
  static char s[1000];
  int status;
  int i;

  //recv from clients
  status = SDLNet_UDP_Recv(hostsock,pkt);
  switch( status ){
    case -1:
      SJC_Write("Network Error: Recv failed!");
      SJC_Write(SDL_GetError());
      break;
    case 1:
      SJC_Write("Recv'd: len=%d | ipv4=%d | port=%d",pkt->len,pkt->address.host,pkt->address.port);
      SJC_Write("%*s",pkt->len,pkt->data);
      for(i=0;i<maxclients;i++)
        if( clients[i].addr.host==pkt->address.host && clients[i].addr.port==pkt->address.port )
          break;
      if( i==maxclients ){ //a new client is connecting?
        host_welcome();
        break;
      }
      //clients[i] has something to say!
      SJC_Write("Known client %d identified.",i);
      //TODO
      break;
  }

  //send to clients
  //TODO
}


//accept a new client and store the "connection"
void host_welcome(){
  int i;
  char *p = pkt->data;
  if( strncmp(p,PROTONAME,strlen(PROTONAME)) ){
    SJC_Write("Junk packet from unknown client.");
    return;
  }
  p += strlen(PROTONAME);
  if( *p!='/' ){
    SJC_Write("Malformed packet from unknown client.");
    return;
  }
  p++;
  if( strncmp(p,PROTOVERS,strlen(PROTOVERS)) || pkt->len!=strlen(PROTONAME)+1+strlen(PROTOVERS) ){
    SJC_Write("Wrong protocol version from unknown client.");
    //TODO: inform client of the problem
    return;
  }
  for(i=0;i<maxclients;i++)
    if( clients[i].addr.host==0 ) break;
  if( i==maxclients ){
    SJC_Write("New client not accepted b/c server is full.");
    //TODO: inform client
    return;
  }
  SJC_Write("New client accepted.");
  clients[i].addr = pkt->address;
}


