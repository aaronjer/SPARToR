/**
 **  SPARToR 
 **  Network Game Engine
 **  Copyright (C) 2010  Jer Wilson
 **
 **  See LICENSE for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "SDL.h"
#include "SDL_net.h"
#include "main.h"
#include "console.h"
#include "command.h"
#include "net.h"
#include "host.h"
#include "client.h"


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
      char *port = strtok(NULL," :");
      char *clientport = strtok(NULL," ");
      client_start(hostname,(port?atoi(port):0),(clientport?atoi(clientport):0));
    }else if( strcmp(q,"disconnect")==0 ){
      disconnect();
    }else if( strcmp(q,"reconnect")==0 ){
      disconnect();
      SJC_Write("Not implemented."); //TODO
    }else if( strcmp(q,"slow")==0 ){
      SJC_Write("Speed is now slow");
      ticksaframe = 300;
      jogframebuffer(metafr,surefr);
    }else if( strcmp(q,"fast")==0 ){
      SJC_Write("Speed is now fast");
      ticksaframe = 30;
      jogframebuffer(metafr,surefr);
    }else if( strcmp(q,"help")==0 ){
      SJC_Write("[ HELP ]");
      SJC_Write(" Press your ~ key to open and close this console.");
      SJC_Write(" Here are some commands you can type:");
      SJC_Write("  listen -- start a server");
      SJC_Write("  connect -- connect to a server");
      SJC_Write("  quit -- quit the program");
      SJC_Write(" Control the game with the arrow keys, Z and X");
    }else{
      SJC_Write("Huh?");
    }
  HARDER
  free(p);
}



