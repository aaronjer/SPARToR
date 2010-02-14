

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
  //TODO
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
    return;
  }
  SJC_Write("New client accepted.");
  sprintf((char *)pkt->data,"MWelcome aboard client %d",i);
  pkt->len = strlen((char *)pkt->data)+1;
  if( !SDLNet_UDP_Send(hostsock,-1,pkt) ) {
    SJC_Write("Error: Could not send welcome packet!");
    SJC_Write(SDL_GetError());
    return; 
  }
  clients[i].addr = pkt->address;
  // send state!
  q = packframe(surefr,&n);
  SJC_Write("Frame %d packed into %d bytes, ready to send state.",surefr,n);
  if( n+4>pkt->maxlen ) {
    SJC_Write("Error: Packed frame is too big to send!");
    return;
  }
  pkt->len = n+5;
  pkt->data[0] = '?';
  packbytes(pkt->data+0,'S',NULL,1);
  packbytes(pkt->data+1,surefr,NULL,4);
  memcpy(pkt->data+5,q,n);
  if( !SDLNet_UDP_Send(hostsock,-1,pkt) ) {
    SJC_Write("Error: Could not send state packet!");
    SJC_Write(SDL_GetError());
    return;
  }

  //TODO: lots
}


Uint8 *packframe(Uint32 packfr,size_t *n) {
  int i;
  size_t s = maxclients*8;
  Uint8 *data = malloc(s);
  *n = 0;

  FRAME_t *pfr = fr + packfr % maxframes;
  packbytes(data,maxclients,n,4);
  for(i=0;i<maxclients;i++) {
    packbytes(data,pfr->cmds[i].cmd    ,n,1);
    packbytes(data,pfr->cmds[i].mousehi,n,1);
    packbytes(data,pfr->cmds[i].mousex ,n,1);
    packbytes(data,pfr->cmds[i].mousey ,n,1);
    packbytes(data,pfr->cmds[i].flags  ,n,2);
  }
  packbytes(data,maxobjs,n,4);
  for(i=0;i<maxobjs;i++) {
    while( *n+4+sizeof(size_t)+pfr->objs[i].size >= s-1 )
      data = realloc(data,(s*=2));
    packbytes(data,pfr->objs[i].type,n,2);
    if(pfr->objs[i].type) {
      packbytes(data,pfr->objs[i].flags,n,2);
      packbytes(data,pfr->objs[i].size ,n,sizeof(size_t));
      memcpy(data, pfr->objs[i].data, pfr->objs[i].size);
    }
  }
  return data;
}


void packbytes(Uint8 *data,Uint64 value,size_t *offset,int width)
{
  size_t n = 0;
  if( offset==NULL ) offset = &n;
  switch(width)
  {
    case 8: *(data+(*offset)++) = (Uint8)(value>>56);
    case 7: *(data+(*offset)++) = (Uint8)(value>>48);
    case 6: *(data+(*offset)++) = (Uint8)(value>>40);
    case 5: *(data+(*offset)++) = (Uint8)(value>>32);
    case 4: *(data+(*offset)++) = (Uint8)(value>>24);
    case 3: *(data+(*offset)++) = (Uint8)(value>>16);
    case 2: *(data+(*offset)++) = (Uint8)(value>>8 );
    case 1: *(data+(*offset)++) = (Uint8)(value    );
  }
}


