
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "main.h"
#include "console.h"
#include "command.h"


void client(){
  UDPpacket pkt;
  char s[1000];
  int status;

  pkt.data = malloc(1000);
  pkt.maxlen = 1000;
/*
  //look for new connections
  status = SDLNet_UDP_Recv(hostsocket,&pkt);
  switch( status ){
    case -1:
      SJC_Write("Network Error: Failed to check for new connections.");
      SJC_Write(SDL_GetError());
      break;
    case 1:
      SJC_Write("Received packet...");
      sprintf(s,"channel %d|maxlen %d|len %d|ipv4 %d|port %d",pkt.channel,pkt.maxlen,pkt.len,pkt.address.host,pkt.address.port);
      SJC_Write(s);
      strncpy(s,pkt.data,pkt.len);
      s[pkt.len]='\0';
      SJC_Write(s);
      break;
  }
*/
  free(pkt.data);
}



