
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "main.h"
#include "console.h"
#include "command.h"
#include "net.h"


void command(const char *s){
  char *p, *q;

  p = malloc(strlen(s)+1);
  strcpy(p,s);
  q = strtok(p," ");
  TRY
    if( q==NULL ){
      ;
    }else if( strcmp(q,"exit")==0 || strcmp(q,"quit")==0 ){
      cleanup();
    }else if( strcmp(q,"listen")==0 ){
      char *port = strtok(NULL," ");
      host_start(port?atoi(port):0);
    }else if( strcmp(q,"connect")==0 ){
      char *hostname = strtok(NULL," :");
      char *port = strtok(NULL," ");
      client_start(hostname,(port?atoi(port):0));
    }else if( strcmp(q,"disconnect")==0 ){
      disconnect();
    }else if( strcmp(q,"reconnect")==0 ){
      disconnect();
      SJC_Write("Not implemented."); //TODO
    }else{
      SJC_Write("Huh?");
    }
  HARDER
  free(p);
}



