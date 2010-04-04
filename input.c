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
  if( !cmd )
    return;
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


void kbinput(int press,SDL_keysym keysym) {
  SDLKey sym = keysym.sym;
  SDLMod mod = keysym.mod;
  Uint16 unicode = keysym.unicode;

  if( (sym==SDLK_q && mod&KMOD_CTRL) || (sym==SDLK_F4 && mod&KMOD_ALT) )
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
  } else
    putcmd( mod_key2cmd(INP_KEYB,sym,press) );
}


void joyinput(int press,SDL_JoyButtonEvent jbutton) {
  putcmd( mod_key2cmd(INP_JBUT,jbutton.button,press) );
}


void axisinput(SDL_JoyAxisEvent jaxis) {
  static char **axdats = NULL;
  static int size = 0;
  static const char POS_ON  = 1;
  static const char NEG_ON  = 2;
  if( size<=jaxis.which ) //haven't seen a joystick this high before?
  {
    axdats = realloc(axdats,sizeof(*axdats)*(jaxis.which+1));
    memset(axdats+size,0,sizeof(*axdats)*(jaxis.which+1-size));
    size = jaxis.which+1;
  }
  if( !axdats[jaxis.which] ) //haven't seen this exact joystick before?
    axdats[jaxis.which] = calloc(256,sizeof(**axdats));
  char *stat = axdats[jaxis.which]+jaxis.axis;

  if( jaxis.value> 3400 && !(*stat&POS_ON) ) { *stat|= POS_ON; putcmd( mod_key2cmd(INP_JAXP,jaxis.axis,1) ); }
  if( jaxis.value< 3000 &&  (*stat&POS_ON) ) { *stat&=~POS_ON; putcmd( mod_key2cmd(INP_JAXP,jaxis.axis,0) ); }
  if( jaxis.value<-3400 && !(*stat&NEG_ON) ) { *stat|= NEG_ON; putcmd( mod_key2cmd(INP_JAXN,jaxis.axis,1) ); }
  if( jaxis.value>-3000 &&  (*stat&NEG_ON) ) { *stat&=~NEG_ON; putcmd( mod_key2cmd(INP_JAXN,jaxis.axis,0) ); }
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

