

#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "main.h"
#include "console.h"
#include "command.h"
#include "host.h"
#include "net.h"


static CLIENT_t *clients;


void host_start(int port) {
  if(!port) port=HOSTPORT;
  if( hostsock ) { SJC_Write("Already running as a host. Type disconnect to stop."); return; }
  if( clientsock ) { SJC_Write("Already connected to a host. Type disconnect if that ain't cool."); return; }
  if( !(hostsock = SDLNet_UDP_Open(port)) ) { SJC_Write("Error: Could not open host socket!"); SJC_Write(SDL_GetError()); return; }
  clients = calloc(maxclients,sizeof(CLIENT_t));
  SJC_Write("Host started on port %d.",port);
}


void host_stop() {
  free(clients);
}


void host() {
  int status;
  int i;
  Uint32 ipnum;
  Uint8 *data;
  size_t n;

  //recv from clients
  status = SDLNet_UDP_Recv(hostsock,pkt);
  switch( status ) {
    case -1:
      SJC_Write("Network Error: Recv failed!");
      SJC_Write(SDL_GetError());
      break;
    case 1:
      ipnum = pkt->address.host;
      SJC_Write("Received UPD packet of length %d from %d.%d.%d.%d:%d",pkt->len,
                ipnum%256,(ipnum>>8)%256,(ipnum>>16)%256,(ipnum>>24)%256,pkt->address.port);
      SJC_Write("Client: %.*s",pkt->len,pkt->data);
      for(i=0;i<maxclients;i++)
        if( clients[i].addr.host==pkt->address.host && clients[i].addr.port==pkt->address.port )
          break;
      if( i==maxclients ) { //a new client is connecting?
        host_welcome();
        break;
      }
      //clients[i] has something to say!
      SJC_Write("Known client %d identified.",i);
      //TODO
      break;
  }

  //send to clients
  pkt->data[0] = 'C';
  pkt->data[1] = 0;
  pkt->len = 2;
  for(i=surefr+1;i<=cmdfr;i++) { //scan for dirty frames to send
    if( fr[i%maxframes].dirty ) {
      if( pkt->data[0]>100 ) {
        SJC_Write("DANGER! >100 framesworth of cmds packed!");
        break;
      }
      data = packframecmds(i,&n);
      if( pkt->len+4+n >= pkt->maxlen ) {
        SJC_Write("DANGER! >=pkt->maxlen bytesworth of cmds almost packed!");
        free(data);
        break;
      }
      packbytes(pkt->data+pkt->len,i,NULL,4);
      memcpy(pkt->data+pkt->len+4, data, n);
      free(data);
      pkt->len += 4+n;
      pkt->data[1]++;
      fr[i%maxframes].dirty = 0;
    }
  }
  if( pkt->data[1]>0 ) { //we have packed cmds to send along!
    SJC_Write("Sending cmds packet of length %d",pkt->len);
    for(i=0;i<maxclients;i++) {
      pkt->address = clients[i].addr;
      if( pkt->address.host && !SDLNet_UDP_Send(hostsock,-1,pkt) ) {
        SJC_Write("Error: Could not send cmds packet!");
        SJC_Write(SDL_GetError());
      }
    }
  }
}


//accept a new client and store the "connection"
void host_welcome() {
  int i;
  size_t n;
  char *p = (char *)pkt->data;
  Uint8 *q;
  if( strncmp(p,PROTONAME,strlen(PROTONAME)) ) {
    SJC_Write("Junk packet from unknown client.");
    return;
  }
  p += strlen(PROTONAME);
  if( *p!='/' ) {
    SJC_Write("Malformed packet from unknown client.");
    return;
  }
  p++;
  if( strncmp(p,PROTOVERS,strlen(PROTOVERS)) || pkt->len!=strlen(PROTONAME)+1+strlen(PROTOVERS) ) {
    SJC_Write("Wrong protocol version from unknown client.");
    //TODO: inform client of the problem
    return;
  }
  for(i=0;i<maxclients;i++)
    if( clients[i].addr.host==0 ) break;
  if( i==maxclients ) {
    SJC_Write("New client not accepted b/c server is full.");
    //TODO: inform client
    sprintf((char *)pkt->data,"MFULL: Server is full!");
    pkt->len = strlen((char *)pkt->data)+1;
    SDLNet_UDP_Send(hostsock,-1,pkt);
    return; 
  }
  SJC_Write("New client accepted.");
  clients[i].addr = pkt->address;
  // send state!
  q = packframe(surefr,&n);
  SJC_Write("Frame %d packed into %d bytes, ready to send state.",surefr,n);
  if( n+9>pkt->maxlen ) {
    SJC_Write("Error: Packed frame is too big to send!");
    free(q);
    return;
  }
  pkt->len = n+9;
  pkt->data[0] = '?';
  packbytes(pkt->data+0,'S',NULL,1);
  packbytes(pkt->data+1,metafr,NULL,4);
  packbytes(pkt->data+5,surefr,NULL,4);
  memcpy(pkt->data+9,q,n);
  if( !SDLNet_UDP_Send(hostsock,-1,pkt) ) {
    SJC_Write("Error: Could not send state packet!");
    SJC_Write(SDL_GetError());
    free(q);
    return;
  }

  free(q);

  //TODO: lots
}


