
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "main.h"
#include "console.h"
#include "command.h"
#include "client.h"
#include "net.h"


static Uint32 sentfr = 0;
static int negotiated;


void client_start(const char *hostname,int port,int clientport) {
  if( hostsock ) { SJC_Write("Already running as a host. Type disconnect to stop."); return; }
  if( clientsock ) { SJC_Write("Already connected to a host. Type disconnect if that ain't cool."); return; }
  if( !hostname || !*hostname ) { SJC_Write("Error: Please specify host."); return; }
  SDLNet_ResolveHost(&ipaddr,hostname,port?port:HOSTPORT);
  if( ipaddr.host==INADDR_NONE ) { SJC_Write("Error: Could not resolve host!"); return; }
  if( !(clientsock = SDLNet_UDP_Open(clientport?clientport:CLIENTPORT)) ) { SJC_Write("Error: Could not open client socket!"); SJC_Write(SDL_GetError()); return; }
  SJC_Write("Connecting...");
  negotiated = 0;
  pkt->address = ipaddr;
  sprintf((char *)pkt->data,"%s/%s",PROTONAME,PROTOVERS);
  pkt->len = strlen((char *)pkt->data);
  if( !SDLNet_UDP_Send(clientsock,-1,pkt) ) {
    SJC_Write("Error: Could not send connect packet!");
    SJC_Write(SDL_GetError());
    SDLNet_UDP_Close(clientsock);
    clientsock = NULL;
  }
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
      SJC_Write("Sending cmd update for dirty frame %d",sentfr);
      pkt->len = 9;
      packbytes(pkt->data+0,'c'                           ,NULL,1);
      packbytes(pkt->data+1,sentfr                        ,NULL,4);
      packbytes(pkt->data+5,fr[sentfrmod].cmds[me].cmd    ,NULL,1);
      packbytes(pkt->data+6,fr[sentfrmod].cmds[me].mousehi,NULL,1);
      packbytes(pkt->data+7,fr[sentfrmod].cmds[me].mousex ,NULL,1);
      packbytes(pkt->data+8,fr[sentfrmod].cmds[me].mousey ,NULL,1);
      if( !SDLNet_UDP_Send(clientsock,-1,pkt) ) {
        SJC_Write("Error: Could not send cmd update packet!");
        SJC_Write(SDL_GetError());
      }
      fr[sentfrmod].dirty = 0; //TODO: really share this var?
    }
  }

  //look for data from server
  status = SDLNet_UDP_Recv(clientsock,pkt);
  if( status==-1 ) {
    SJC_Write("Network Error: Failed to check for new packets.");
    SJC_Write(SDL_GetError());
  } else if( status==1 ) {
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
          unpackframecmds(packfr,pkt->data+n,pkt->len-n);
          if( hotfr>packfr-1 )
            sethotfr(packfr-1);
        }
        break;
      default:
        SJC_Write("Error: Packet is garbled!");
    }
  }
}



