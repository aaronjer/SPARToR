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


void input(int press,SDL_keysym keysym) {
  SDLKey sym = keysym.sym;
  SDLMod mod = keysym.mod;
  Uint16 unicode = keysym.unicode;

  if( (sym==SDLK_q  && mod&(KMOD_LCTRL|KMOD_RCTRL)) ||
      (sym==SDLK_F4 && mod&(KMOD_LALT |KMOD_RALT )) )
    command("exit");
  else if(press && sym==SDLK_F11) {
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
    if( (cmd = getnextcmd()) ) { // dirty frame if new cmd inserted
      fr[infr].cmds[me].cmd = cmd;
      fr[infr].dirty = 1;
    }
  }
}


