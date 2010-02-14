
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "main.h"
#include "console.h"
#include "command.h"
#include "client.h"
#include "net.h"


void client_start(const char *hostname,int port) {
  if( hostsock ) { SJC_Write("Already running as a host. Type disconnect to stop."); return; }
  if( clientsock ) { SJC_Write("Already connected to a host. Type disconnect if that ain't cool."); return; }
  if( !hostname || !*hostname ) { SJC_Write("Error: Please specify host."); return; }
  SDLNet_ResolveHost(&ipaddr,hostname,port?port:HOSTPORT);
  if( ipaddr.host==INADDR_NONE ) { SJC_Write("Error: Could not resolve host!"); return; }
  if( !(clientsock = SDLNet_UDP_Open(CLIENTPORT)) ) { SJC_Write("Error: Could not open client socket!"); SJC_Write(SDL_GetError()); return; }
  SJC_Write("Connecting...");
  pkt->address = ipaddr;
  sprintf((char *)pkt->data,"%s/%s",PROTONAME,PROTOVERS);
  pkt->len = strlen((char *)pkt->data);
  if( !SDLNet_UDP_Send(clientsock,-1,pkt) ) {
    SJC_Write("Error: Could not send connect packet!");
    SJC_Write(SDL_GetError());
    SDLNet_UDP_Close(clientsock);
    clientsock = NULL;
  }
  //SDLNet_UDP_Send(clientsock,-1,pkt);
}


void client() {
  int status;

  //look for new connections, bub
  status = SDLNet_UDP_Recv(clientsock,pkt);
  if( status==-1 ) {
    SJC_Write("Network Error: Failed to check for new packets.");
    SJC_Write(SDL_GetError());
  } else if( status==1 ) {
    Uint32 ipnum = pkt->address.host;
    SJC_Write("Received UPD packet of length %d from %d.%d.%d.%d:%d",pkt->len,
              ipnum%256,(ipnum>>8)%256,(ipnum>>16)%256,(ipnum>>24)%256,pkt->address.port);
    switch(pkt->data[0]) {
      case 'M': //message
        SJC_Write("Server says: %s",pkt->data+1);
        break;
      case 'S': //state
        SJC_Write("Receiving state, %d bytes",pkt->len-5);
        unpackframe(0,pkt->data+5);
        break;
      default:
        SJC_Write("Error: Packet is garbled!");
    }
  }
}



