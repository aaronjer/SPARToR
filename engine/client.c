/**
 **  SPARToR 
 **  Network Game Engine
 **  Copyright (C) 2010-2011  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "SDL.h"
#include "SDL_net.h"
#include "mod.h"
#include "main.h"
#include "console.h"
#include "command.h"
#include "client.h"
#include "host.h"
#include "net.h"


UDPsocket clientsock = NULL;


static Uint32 pktnum;
static Uint32 sentfr = 0;
static int negotiated;
static UDPpacket *pkt;


void client_start(const char *hostname,int port,int clientport) {
  int i;
  IPaddress ipaddr;

  pktnum = 1;
  if( !hostname || !*hostname ) {  SJC_Write("Usage: connect <host> [<port>] [<localport>]");          return; }
  if( hostsock ) {                 SJC_Write("Already running as a host. Type 'disconnect' to stop."); return; }
  if( clientsock ) {               SJC_Write("Already connected to a host. Type 'disconnect' first."); return; }
  SDLNet_ResolveHost(&ipaddr,hostname,port?port:HOSTPORT);
  if( ipaddr.host==INADDR_NONE ) { SJC_Write("Error: Could not resolve host!");                        return; }
  clientport = clientport?clientport:CLIENTPORT;
  for(i=0;i<maxclients;i++) {
    if( (clientsock = SDLNet_UDP_Open(clientport)) )
      break;
    SJC_Write("Could not open client socket on port %d",clientport++);
    SJC_Write(SDL_GetError());
  }
    
  if( !clientsock ) { SJC_Write("Error: Could not open client any client socket!"); return; }
  SJC_Write("Connecting from port %d...",clientport);
  negotiated = 0;
  pkt = SDLNet_AllocPacket(PACKET_SIZE);
  pkt->address = ipaddr;
  sprintf((char *)pkt->data,"%s/%s",PROTONAME,VERSION);
  pkt->len = strlen((char *)pkt->data);
  if( !SDLNet_UDP_Send(clientsock,-1,pkt) ) {
    SJC_Write("Error: Could not send connect packet!");
    SJC_Write(SDL_GetError());
    client_stop();
  }
}


void client_stop() {
  SDLNet_FreePacket(pkt);
  SDLNet_UDP_Close(clientsock);
  clientsock = NULL;
}


void client() {
  int status;
  int i;
  size_t n;
  Uint32 packfr;

  //send cmd updates to server
  while(negotiated && sentfr<metafr-1) {
    sentfr++;
    Uint32 sentfrmod = sentfr%maxframes;
    if( fr[sentfrmod].dirty ) {
      n = 0;
      FCMD_t *c = fr[sentfrmod].cmds+me;
      packbytes(pkt->data,'c'             ,&n,1);
      packbytes(pkt->data,pktnum++        ,&n,4);
      packbytes(pkt->data,sentfr          ,&n,4);
      packbytes(pkt->data,c->cmd          ,&n,1);
      packbytes(pkt->data,c->mousehi      ,&n,1);
      packbytes(pkt->data,c->mousex       ,&n,1);
      packbytes(pkt->data,c->mousey       ,&n,1);
      packbytes(pkt->data,c->flags        ,&n,2);
      if( c->flags & CMDF_DATA ) {
        packbytes(pkt->data,c->datasz     ,&n,2);
        memcpy(pkt->data+n,c->data,c->datasz);
        n += c->datasz;
      }
      pkt->len = n;
      if( !SDLNet_UDP_Send(clientsock,-1,pkt) ) {
        SJC_Write("Error: Could not send cmd update packet!");
        SJC_Write(SDL_GetError());
      }
      fr[sentfrmod].dirty = 0; //TODO: really share this var?
    }
  }

  //look for data from server
  for(;;) {
    status = SDLNet_UDP_Recv(clientsock,pkt);
    if( status==-1 ) {
      SJC_Write("Network Error: Failed to check for new packets.");
      SJC_Write(SDL_GetError());
    }
    if( status!=1 )
      break;
    switch(pkt->data[0]) {
      case 'M': //message
        SJC_Write("Server says: %s",pkt->data+1);
        break;
      case 'S': //state
        clearframebuffer();
        me               = unpackbytes(pkt->data+1,4,NULL,1);
        Uint32 newmetafr = unpackbytes(pkt->data+2,4,NULL,4);
        Uint32 newsurefr = unpackbytes(pkt->data+6,4,NULL,4);
        sentfr = newmetafr-1;
        jogframebuffer( newmetafr, newsurefr );
        SJC_Write("Receiving state of frame %d, %d bytes, syncing up at frame %d as client %d",
                  surefr,pkt->len-10,metafr,me);

        unpackframe(surefr,pkt->data+10,pkt->len-10);
        negotiated = 1;
        break;
      case 'C': //cmds
        n = 2;
        for(i=0;i<(int)pkt->data[1];i++) {
          packfr = unpackbytes(pkt->data,pkt->len,&n,4);
          setcmdfr(packfr);
          int unpacked = unpackframecmds(packfr,pkt->data+n,pkt->len-n);
          if( unpacked<0 ) {
            SJC_Write("Failed to unpack frame cmds!");
            break;
          }
          n += unpacked;
          if( hotfr>packfr-1 )
            sethotfr(packfr-1);
        }
        break;
      default:
        SJC_Write("Error: Packet is garbled!");
    }
  }
}



