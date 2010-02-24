/**
 **  SPARToR 
 **  Network Game Engine
 **  Copyright (C) 2010  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "SDL.h"
#include "SDL_net.h"
#include "input.h"
#include "main.h"
#include "console.h"
#include "net.h"
#include "video.h"
#include "command.h"
#include "mod.h"


static char cmdbuf[256] = {0};
static int cbwrite = 0;
static int cbread = 0;


void putcmd(char cmd) {
  cmdbuf[cbwrite] = cmd;
  cbwrite = (cbwrite+1)%256;
}


char getnextcmd() {
  char cmd = cmdbuf[cbread];
  if( cmd==0 ) return 0;
  cmdbuf[cbread] = 0;
  cbread = (cbread+1)%256;
  return cmd;
}


void input(int press,int sym,Uint16 unicode) {
  if(press && sym==SDLK_F11) {
    if( !fullscreen )
      command("fullscreen");
    else
      command("window");
  } else if(press && sym==SDLK_BACKQUOTE)
    toggleconsole();
  else if(press && console_open) {
    if(unicode>31 && unicode<127)
      SJC_Put((char)unicode);
    else if(sym==SDLK_RETURN) {
      if( SJC_Submit() )
        command(SJC.buf[1]);
    }
    else if(sym==SDLK_BACKSPACE || sym==SDLK_DELETE)
      SJC_Rub();
    else if(sym==SDLK_ESCAPE && console_open)
      toggleconsole();
  } else {
    char cmd = mod_key2cmd(sym,press);
    if( cmd )
      putcmd(cmd);
  }
}


void readinput() {
  Uint32 infr = hotfr+1; //TODO: _should_ we always insert on hotfr+1?
  if( cmdfr<infr ) //this is the new cmdfr, so clear it, unless we already have cmds stored in the future!
    setcmdfr(infr);
  infr %= maxframes;
  if( fr[infr].cmds[me].cmd==0 && cmdbuf[cbread] ) {
    char cmd;
/*  int i; //FIXME: remove
    char s[257];
    char t[257];
    for(i=0;i<256;i++) {
      s[i] = cmdbuf[i]?cmdbuf[i]+'0':'-';
      t[i] = ' ';
    }
    t[cbread] = 'R';
    t[cbwrite] = 'W';
    s[256] = '\0';
    t[256] = '\0';
    SJC_Write(s);
    SJC_Write(t); */
    if( (cmd = getnextcmd()) ) { // dirty frame if new cmd inserted
      fr[infr].cmds[me].cmd = cmd;
      fr[infr].dirty = 1;
    }
  }
}


