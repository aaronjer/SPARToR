

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
  me = 0;
  clients[me].connected = 1;
  clients[me].addr = (IPaddress){0,0};
  SJC_Write("Host started on port %d.",port);
}


void host_stop() {
  free(clients);
}


void host() {
  int status;
  int i;
  Uint8 *data;
  size_t n;
  Uint32 packfr;
  FCMD_t *pcmd;

  //recv from clients
  for(;;) {
    status = SDLNet_UDP_Recv(hostsock,pkt);
    if( status==-1 ) {
      SJC_Write("Network Error: Recv failed!");
      SJC_Write(SDL_GetError());
    }
    if( status!=1 )
      break;
    for(i=0;i<maxclients;i++)
      if(clients[i].connected &&
         clients[i].addr.host==pkt->address.host &&
         clients[i].addr.port==pkt->address.port)
        break;
    if(i==maxclients) //a new client is connecting?
      host_welcome();
    else switch(pkt->data[0]) { //clients[i] has something to say!
      case 'm': //message
        SJC_Write("Client %d says: %.*s",i,pkt->len,pkt->data);
        break;
      case 'c': //cmd update
        n = 1;
        packfr = unpackbytes(pkt->data,pkt->len,&n,4);
        setcmdfr(packfr); //FIXME: totally just trusting the client here!
        if( hotfr>packfr-1 )
          sethotfr(packfr-1);
        fr[packfr%maxframes].dirty = 1;
        pcmd = fr[packfr%maxframes].cmds+i;
        pcmd->cmd     = unpackbytes(pkt->data,pkt->len,&n,1);
        pcmd->mousehi = unpackbytes(pkt->data,pkt->len,&n,1);
        pcmd->mousex  = unpackbytes(pkt->data,pkt->len,&n,1);
        pcmd->mousey  = unpackbytes(pkt->data,pkt->len,&n,1);
        break;
    }
  }

  //send to clients
  pkt->data[0] = 'C';
  pkt->data[1] = 0;
  pkt->len = 2;
  for(i=surefr+1;i<=cmdfr;i++) { //scan for dirty frames to send
    if( fr[i%maxframes].dirty ) {
      data = packframecmds(i,&n);
      if( pkt->len+4+n >= pkt->maxlen || pkt->data[1]>100 ) {
        SJC_Write("%d: Packed too many cmds! Will get the rest next frame...",hotfr);
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
    for(i=0;i<maxclients;i++) {
      pkt->address = clients[i].addr;
      if( clients[i].connected && pkt->address.host && !SDLNet_UDP_Send(hostsock,-1,pkt) ) {
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
    if( !clients[i].connected ) break;
  if( i==maxclients ) {
    SJC_Write("New client not accepted b/c server is full.");
    //TODO: inform client
    sprintf((char *)pkt->data,"MFULL: Server is full!");
    pkt->len = strlen((char *)pkt->data)+1;
    SDLNet_UDP_Send(hostsock,-1,pkt);
    return; 
  }
  SJC_Write("New client accepted.");
  clients[i].connected = 1;
  clients[i].addr = pkt->address;
  setcmdfr(metafr);
  sethotfr(metafr-1);
  fr[metafr%maxframes].dirty = 1;
  fr[metafr%maxframes].cmds[i].flags |= CMDF_NEW;
  // send state!
  q = packframe(surefr,&n);
  SJC_Write("Frame %d packed into %d bytes, ready to send state.",surefr,n);
  if( n+10>pkt->maxlen ) {
    SJC_Write("Error: Packed frame is too big to send!");
    free(q);
    return;
  }
  pkt->len = n+10;
  packbytes(pkt->data+0,   'S',NULL,1);
  packbytes(pkt->data+1,     i,NULL,1);
  packbytes(pkt->data+2,metafr,NULL,4);
  packbytes(pkt->data+6,surefr,NULL,4);
  memcpy(pkt->data+10,q,n);
  if( !SDLNet_UDP_Send(hostsock,-1,pkt) ) {
    SJC_Write("Error: Could not send state packet!");
    SJC_Write(SDL_GetError());
    free(q);
    return;
  }
  //dirty all unsure frames
  SJC_Write("%d: Dirtying all frames from %d to %d",hotfr,surefr,cmdfr);
  for(i=surefr+1;i<cmdfr;i++)
    fr[i%maxframes].dirty = 1;

  free(q);

  //TODO: lots
}


