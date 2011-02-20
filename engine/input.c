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
#include "input.h"
#include "main.h"
#include "console.h"
#include "net.h"
#include "video.h"
#include "command.h"

static int  kwik = 0;
static char kwik_presscmd;
static char kwik_releasecmd;

static char cmdbuf[250] = {0};
static int  cbwrite = 0;
static int  cbread = 0;

static SDL_Joystick **joysticks;


void inputinit() {
  int i, numjoysticks;
  if( (numjoysticks=SDL_NumJoysticks())>0 ) {
    joysticks = malloc(sizeof(*joysticks)*numjoysticks);
    SDL_JoystickEventState(SDL_ENABLE);
    SJC_Write("%d controller/joystick%s detected:",numjoysticks,(numjoysticks>1?"s":""));
    for( i=0; i<numjoysticks; i++ ) {
      joysticks[i] = SDL_JoystickOpen(i);
      SJC_Write("  #%i: %.20s",i,SDL_JoystickName(i));
    }
  }
}


void putcmd(char cmd) {
  if( !cmd || cbread%250==(cbwrite+1)%250 )
    return;
  cmdbuf[cbwrite] = cmd;
  cbwrite = (cbwrite+1)%250;
}

char getnextcmd() {
  char cmd = cmdbuf[cbread];
  if( cmd==0 ) return 0;
  cmdbuf[cbread] = 0;
  cbread = (cbread+1)%250;
  return cmd;
}


void kbinput(int press,SDL_keysym keysym) {
  SDLKey sym = keysym.sym;
  SDLMod mod = keysym.mod;
  Uint16 unicode = keysym.unicode;

  if( (sym==SDLK_q && mod&(KMOD_CTRL|KMOD_META)) || (sym==SDLK_F4 && mod&KMOD_ALT) )
    command("exit");
  else if( press && (sym==SDLK_F11 || (sym==SDLK_f && mod&KMOD_META)) ) {
    if( !v_fullscreen )
      command("fullscreen");
    else
      command("window");
  } else if(press && sym==SDLK_BACKQUOTE)
    toggleconsole();
  else if(press && kwik)
    kwikbind( INP_KEYB, sym );
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
  if( kwik && press )
    kwikbind( INP_JBUT, jbutton.button );
  else
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
  int val = jaxis.value;
  int ax = jaxis.axis;

  if( val> 3400 && !(*stat&POS_ON) ) { *stat|= POS_ON; kwik ? kwikbind(INP_JAXP,ax) : putcmd( mod_key2cmd(INP_JAXP,ax,1) ); }
  if( val< 3000 &&  (*stat&POS_ON) ) { *stat&=~POS_ON;                                putcmd( mod_key2cmd(INP_JAXP,ax,0) ); }
  if( val<-3400 && !(*stat&NEG_ON) ) { *stat|= NEG_ON; kwik ? kwikbind(INP_JAXN,ax) : putcmd( mod_key2cmd(INP_JAXN,ax,1) ); }
  if( val>-3000 &&  (*stat&NEG_ON) ) { *stat&=~NEG_ON;                                putcmd( mod_key2cmd(INP_JAXN,ax,0) ); }
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


void input_bindsoon(int presscmd,int releasecmd) {
  kwik_presscmd = presscmd;
  kwik_releasecmd = releasecmd;
  kwik = 1;
}

void kwikbind(int device,int sym) {
  mod_keybind( device, sym, 0, kwik_releasecmd );
  mod_keybind( device, sym, 1, kwik_presscmd );
  kwik = 0;
  switch( device ) {
    case INP_KEYB: SJC_Write("Key #%d selected"                ,sym); break;
    case INP_JBUT: SJC_Write("Button #%d selected"             ,sym); break;
    case INP_JAXP: SJC_Write("Axis #%d (+) selected"           ,sym); break;
    case INP_JAXN: SJC_Write("Axis #%d (-) selected"           ,sym); break;
    default:       SJC_Write("Unkown device input #%d selected",sym); break;
  }
}



