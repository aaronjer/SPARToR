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
#include "net.h"
#include "host.h"
#include "client.h"
#include "video.h"
#include "input.h"
#include "saveload.h"


void command(const char *s){
  char *p, *q;

  p = malloc(strlen(s)+1);
  strcpy(p,s);
  q = strtok(p," ");
  TRY
    if( q==NULL ){
      ;
    }else if( strcmp(q,"realtime")==0 ){
      eng_realtime = eng_realtime ? 0 : 1;
      SJC_Write("Realtime mode %s",eng_realtime?"on":"off");
    }else if( strcmp(q,"watch")==0 ){
      i_watch = i_watch ? 0 : 1;
      SJC_Write("Input watch %s",i_watch?"on":"off");
    }else if( strcmp(q,"alpha")==0 ){
      char *val = strtok(NULL," ");
      if( val ) v_usealpha = atoi(val);
    }else if( strcmp(q,"oob")==0 ){
      char *val = strtok(NULL," ");
      if( val ) v_oob = atoi(val);
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
      if( hostsock ){
        host_stop();
        SJC_Write("Host stopped.");
      }else if( clientsock ){
        client_stop();
        SJC_Write("Disconnected from host.");
      }else
        SJC_Write("Nothing to disconnect from.");
    }else if( strcmp(q,"reconnect")==0 ){
      SJC_Write("Not implemented."); //TODO
    }else if( strcmp(q,"hulls")==0 ){
      v_drawhulls = v_drawhulls ? 0 : 1;
    }else if( strcmp(q,"stats")==0 ){
      v_showstats = v_showstats ? 0 : 1;
    }else if( strcmp(q,"fullscreen")==0 || strncmp(q,"window",6)==0 ){
      char *sw = strtok(NULL," x");
      char *sh = strtok(NULL," ");
      int w = sw?atoi(sw):0;
      int h = sh?atoi(sh):0;
      int full = strcmp(q,"fullscreen")==0 ? 1 : 0;
      if( w>=320 && h>=200 )
        setvideosoon(w,h,full,1);
      else if( w>=1 && w<=5 )
        setvideosoon(NATIVEW*w,NATIVEH*w,full,1);
      else
        setvideosoon(0,0,full,1);
    }else if( strcmp(q,"bind")==0 ){
      int i;
      char *cmdname = strtok(NULL," ");
      if( cmdname==NULL ) {
        SJC_Write("Please specify command: left, right, up, down, fire, jump");
        break;
      }
      for(i=0; i<numinputnames; i++)
        if( strcmp(inputnames[i].name,cmdname)==0 ) {
          input_bindsoon( inputnames[i].presscmd, inputnames[i].releasecmd );
          SJC_Write("Press a key, button, or stick to use for [%s] ...",cmdname);
          break;
        }
      if( i==numinputnames )
        SJC_Write("Not a command: %s",cmdname);
    }else if( strcmp(q,"slow")==0 ){
      SJC_Write("Speed is now slow");
      ticksaframe = 300;
      jogframebuffer(metafr,surefr);
    }else if( strcmp(q,"fast")==0 ){
      SJC_Write("Speed is now fast");
      ticksaframe = 30;
      jogframebuffer(metafr,surefr);
    }else if( strcmp(q,"help")==0 ){
      SJC_Write("Press your ~ key to open and close this console. Commands you can type:");
      SJC_Write("     listen -- start a server");
      SJC_Write("     connect -- connect to a server");
      SJC_Write("     fullscreen 1024 768 -- go fullscreen at 1024x768");
      SJC_Write("     window 3x -- go windowed at 3x up-scale");
      SJC_Write("     bind -- choose input keys");
      SJC_Write("Default controls: Z, X, ARROWS, F11");
    }else if( strcmp(q,"report")==0 ){
      int i;
      for( i=0; i<maxobjs; i++ ) {
        OBJ_t *o = fr[surefr%maxframes].objs+i;
        if( o->type )
          SJC_Write( "#%i %s C:%i F:%x", i, objectnames[o->type], o->context, o->flags );
      }
    }else if( strcmp(q,"save")==0 ){
      char *name = strtok(NULL," ");
      if( name==NULL ) {
        SJC_Write("Please specify a file name to save");
        break;
      }
      save_context( name, mycontext, hotfr );
    }else if( strcmp(q,"load")==0 ){
      char *name = strtok(NULL," ");
      if( name==NULL ) {
        SJC_Write("Please specify a file name to load");
        break;
      }
      load_context( name, mycontext, hotfr );
    }else if( mod_command(q) ){
      SJC_Write("Huh?");
    }
  HARDER
  free(p);
}



