/**
 **  SPARToR [CORE]
 **  Network Game Engine
 **  by SuperJer
 **
 **  www.superjer.com
 **  www.spartor.com
 **/


#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "SDL/SDL_image.h"
#include "main.h"
#include "font.h"
#include "console.h"
#include "net.h"
#include "host.h"
#include "client.h"
#include "sjui.h"
#include "mod.h"
#include "input.h"
#include "video.h"

#ifndef IMG_Init //support pre SDL_image 1.2.8
#define IMG_INIT_PNG 1
int IMG_Init(int flags) {return flags;}
void IMG_Quit() {}
#endif

//globals
Uint32 ticksaframe = TICKSAFRAME;
size_t maxframes = 360;
size_t maxobjs = 100;

FRAME_t *fr;
Uint32 frameoffset;
Uint32 metafr;
Uint32 surefr;
Uint32 drawnfr;
Uint32 hotfr;
Uint32 cmdfr = 1; //DO NOT clear frame 1, it is prefilled with client-connect for local person
int creatables;

SDL_Surface *screen;
Uint32 ticks,newticks;
int me;
int console_open;
UDPpacket *pkt;



int main(int argc,char **argv) {
  SDL_Event event;
  const SDL_VideoInfo *vidinfo;
  int i;

  fr = calloc(sizeof(FRAME_t),maxframes);
  for(i=0;i<maxframes;i++) {
    fr[i].cmds = calloc(sizeof(FCMD_t),maxclients);
    fr[i].objs = calloc(sizeof(OBJ_t),maxobjs);
  }
  //make the mother object
  fr[0].objs[0] = (OBJ_t){OBJT_MOTHER,0,0,0};
  //server is a client
  fr[1].cmds[0].flags = CMDF_LIV|CMDF_NEW;

  if( SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO)<0 || !SDL_GetVideoInfo() ) {
    fprintf(stderr,"SDL_Init: %s\n",SDL_GetError());
    exit(-1);
  }
  if( SDLNet_Init()<0 ) {
    fprintf(stderr,"SDLNet_Init: %s\n",SDL_GetError());
    exit(-2);
  }
  if( IMG_Init(IMG_INIT_PNG)!=IMG_INIT_PNG ) {
    fprintf(stderr,"IMG_Init: %s\n",SDL_GetError());
    exit(-3);
  }
  pkt = SDLNet_AllocPacket(PACKET_SIZE);
  toggleconsole();
  SDL_WM_SetCaption("SPARToR CORE",NULL);
  SDL_WM_SetIcon(IMG_Load("icon.png"),NULL);
  setvideo(640,480);
  vidinfo = SDL_GetVideoInfo();

  SJF_Init();

  //UI
  SJUI_Init();
  SJUI_HANDLE h = SJUI_NewControl(0,50,90,0);
  SJUI_SetPos(h,10,10);

  SJC_Write("SPARToR CORE v%s",VERSION);

  //main loop
  for(;;) {
    newticks = SDL_GetTicks();
    if( newticks-ticks<10 && newticks-ticks>=0 ) {
      SDL_Delay(1);
      continue;
    }
    ticks = newticks;
    metafr = ticks/ticksaframe + frameoffset;
    while( SDL_PollEvent(&event) ) switch(event.type) {
      case SDL_VIDEOEXPOSE:                                                         break;
      case SDL_VIDEORESIZE: setvideo(event.resize.w,event.resize.h);                break;
      case SDL_KEYDOWN: input( 1, event.key.keysym.sym, event.key.keysym.unicode ); break;
      case SDL_KEYUP:   input( 0, event.key.keysym.sym, event.key.keysym.unicode ); break;
      case SDL_QUIT: cleanup();                                                     break;
    }
    readinput();
    if(hostsock)   host();
    if(clientsock) client();
    advance();
    render();
  }
  return 0;
}


void toggleconsole() {
  if( console_open ) {
    console_open = 0;
    SDL_EnableUNICODE(0);
    SDL_EnableKeyRepeat(0,0);
  } else {
    console_open = 1;
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
  }
}


void advance() {
  int i;
  findfreeslot(-1);
  while(hotfr < metafr) {
if( hotfr<3 ) SJC_Write("Advance: hotfr=%d, metafr=%d",hotfr,metafr); //FIXME: remove
    sethotfr(hotfr+1);
    Uint32 a = (hotfr-1)%maxframes; //a: frame to advance from, b: frame to advance to
    Uint32 b = (hotfr  )%maxframes;
    if( cmdfr<hotfr ) //need to clear out the cmds in forward frame since it hasn't been done yet!
      setcmdfr(hotfr);
    for(i=0;i<maxobjs;i++) { //first pass
      OBJ_t *oa = fr[a].objs+i;
      OBJ_t *ob = fr[b].objs+i;
      free(ob->data);
      memset(ob,0,sizeof(*ob));
      if(oa->type) {
        ob->type = oa->type;
        ob->flags = oa->flags;
        ob->size = oa->size;
        ob->data = malloc(oa->size);
        memcpy(ob->data,oa->data,oa->size);
      } 
    }
    for(i=0;i<maxobjs;i++) { //second pass
      OBJ_t *oa = fr[a].objs+i;
      OBJ_t *ob = fr[b].objs+i;
      if(oa->type) {
        if(ob->type==OBJT_DUMMY) {
          V *pos = flexpos(ob);
          pos->x += (float)((hotfr-i)%50) - 24.5f;
        }
        mod_adv(i,a,b,oa,ob);
      } 
    }
    for(i=0;i<maxobjs && creatables>0;i++) { //create dummies if requested
      OBJ_t *ob = fr[b].objs+i;
      if(!ob->type) {
        ob->type = OBJT_DUMMY;
        ob->flags = OBJF_POS|OBJF_VIS;
        ob->size = sizeof(V);
        ob->data = malloc(ob->size);
        V *pos = flexpos(ob);
        *pos = (V){320.0f,20.0f+(float)(rand()%440),0.0f};
        creatables--;
        SJC_Write("Created new OBJ_t at frame %d, obj %d, addr %X",b,i,&ob);
      }
    }
    if( creatables>0 ) {
      SJC_Write("Could not create %d objects!",creatables);
      creatables = 0;
    }
    setsurefr(hotfr>50 ? hotfr-50 : 0); //FIXME: UGLY HACK! surefr should be determined for REAL
  }
}


void cleanup() {
  int i;
  SDLNet_FreePacket(pkt);
  SJUI_Destroy();
  IMG_Quit();
  SDLNet_Quit();
  SDL_Quit();
  clearframebuffer();
  for(i=0;i<maxframes;i++) {
    free(fr[i].cmds);
    free(fr[i].objs);
  }
  free(fr);
  exit(0);
}


// find a free obj slot in frame frame1
// frame is "free" as long as it was empty in the previous frame
// staticly remembers which frames have already been given out this way
// if a slot in the current frame is filled by other code it may be CORRUPTED BY USING THIS
int findfreeslot(int frame1) {
  static int last_slot = 0;
  static int last_frame = 0;
  int frame0 = (frame1>0)?(frame1-1):(maxframes-1);
  if( last_frame!=frame1 ) {
    last_frame = frame1;
    last_slot = 1;
  }
  if( frame1==-1 ) //exit early
    return -1;
  while(last_slot<maxobjs) {
    if( fr[frame0].objs[last_slot].type==0 &&
        fr[frame1].objs[last_slot].type==0 ) //empty
      return last_slot++;
    last_slot++;
  }
  return -1;
}

// clears all objects and commands out of frame buffer
void clearframebuffer() {
  int i,j;
  for(i=0;i<maxframes;i++) {
    fr[i].dirty = 0;
    memset(fr[i].cmds,0,sizeof(FCMD_t)*maxclients);
    for(j=0;j<maxobjs;j++) {
      if( fr[i].objs[j].data )
        free( fr[i].objs[j].data );
      memset( fr[i].objs+j, 0, sizeof(OBJ_t) );
    }
  }
}


void assert(const char *msg,int val) {
  if( !val )
    SJC_Write("Assert failed! %s",msg);
}


V *flexpos(OBJ_t *o){
   if(!(o->flags & OBJF_POS))
     return NULL;
   return (V*)(o->data);
}



//frame setters
void setmetafr( Uint32 to) {
  metafr = to;
}
void setsurefr( Uint32 to) {
  surefr = to;
}
void setdrawnfr(Uint32 to) {
  drawnfr = to;
}
void sethotfr(  Uint32 to) {
  hotfr = to;
}
void setcmdfr(  Uint32 to) {
  while(cmdfr<to) {
    cmdfr++;
    memset(fr[cmdfr%maxframes].cmds,0,sizeof(FCMD_t)*maxclients);
    fr[cmdfr%maxframes].dirty = 0;
  }
  if(hotfr>=cmdfr)
    hotfr = cmdfr-1;
  if(surefr>=cmdfr)
    SJC_Write("*** DESYNC: cmdfr has been set = or before surefr! ***");
}
void jogframebuffer(Uint32  newmetafr,Uint32 newsurefr) {
  metafr = newmetafr;
  frameoffset = metafr - ticks/ticksaframe;
  surefr  = newsurefr;
  drawnfr = newsurefr;
  hotfr   = newsurefr;
  cmdfr   = newsurefr;
}


