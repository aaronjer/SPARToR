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
#include "SDL_image.h"
#include "main.h"
#include "font.h"
#include "console.h"
#include "net.h"
#include "host.h"
#include "client.h"
#include "mod.h"
#include "input.h"
#include "video.h"
#include <math.h>

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

Uint32 idle_time = 0;
Uint32 render_time = 0;
Uint32 adv_move_time = 0;
Uint32 adv_collide_time = 0;
Uint32 adv_game_time = 0;
Uint32 adv_frames = 0;



int main(int argc,char **argv) {
  SDL_Event event;
  const SDL_VideoInfo *vidinfo;
  int i;
  Uint32 idle_start = 0;

  fr = calloc(sizeof(FRAME_t),maxframes);
  for(i=0;i<maxframes;i++) {
    fr[i].cmds = calloc(sizeof(FCMD_t),maxclients);
    fr[i].objs = calloc(sizeof(OBJ_t),maxobjs);
  }
  //make the mother object
  fr[0].objs[0] = (OBJ_t){OBJT_MOTHER,0,0,0};
  //server is a client
  fr[1].cmds[0].flags |= CMDF_NEW;

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
  SDL_WM_SetCaption("SPARToR " VERSION,NULL);
  SDL_Surface *iconsurf = IMG_Load("icon.png");
  SDL_WM_SetIcon(iconsurf,NULL);
  SDL_FreeSurface(iconsurf);
  setvideo(640,480);
  vidinfo = SDL_GetVideoInfo();

  SJF_Init();

  SJC_Write("SPARToR v%s  Copyright (C) 2010 Jer Wilson",VERSION);
  SJC_Write(" ***************************");
  SJC_Write("     Type 'help' for help.");
  SJC_Write(" ***************************");
  SJC_Write("");

  //main loop
  for(;;) {
    newticks = SDL_GetTicks();
    if( newticks-ticks<5 ) // give system some time to breathe if we're not too busy
      SDL_Delay(1);
    ticks = newticks;
    metafr = ticks/ticksaframe + frameoffset;
    while( SDL_PollEvent(&event) ) switch(event.type) {
      case SDL_VIDEOEXPOSE:                                                         break;
      case SDL_VIDEORESIZE: setvideo(event.resize.w,event.resize.h);                break;
      case SDL_KEYDOWN: input( 1, event.key.keysym.sym, event.key.keysym.unicode ); break;
      case SDL_KEYUP:   input( 0, event.key.keysym.sym, event.key.keysym.unicode ); break;
      case SDL_QUIT: cleanup();                                                     break;
    }
    idle_time += SDL_GetTicks() - idle_start;
    readinput();
    if(hostsock)   host();
    if(clientsock) client();
    advance();
    render();
    idle_start = SDL_GetTicks();
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
  int i,j,r;
  char recheck[2][maxobjs]; // for collision rechecking
  findfreeslot(-1); // reset slot finder

  while(hotfr < metafr) {
    sethotfr(hotfr+1);
    Uint32 a = (hotfr-1)%maxframes; //a: frame to advance from, b: frame to advance to
    Uint32 b = (hotfr  )%maxframes;
    if( cmdfr<hotfr ) //need to clear out the cmds in forward frame since it hasn't been done yet!
      setcmdfr(hotfr);
    for(i=0;i<maxobjs;i++) { //first pass -- copy forward, move, clip with world
      OBJ_t *oa = fr[a].objs+i;
      OBJ_t *ob = fr[b].objs+i;
      free(ob->data);
      memset(ob,0,sizeof(*ob));
      if(oa->type && !(oa->flags&OBJF_DEL)) {
        ob->type = oa->type;
        ob->flags = oa->flags;
        ob->size = oa->size;
        ob->data = malloc(oa->size);
        memcpy(ob->data,oa->data,oa->size);
      }
      if( (ob->flags & (OBJF_POS|OBJF_VEL)) == (OBJF_POS|OBJF_VEL) ) {
        V *pos  = flex(ob,OBJF_POS);
        V *vel  = flex(ob,OBJF_VEL);
        V *pvel = (ob->flags & OBJF_PVEL) ? flex(ob,OBJF_PVEL) : NULL;
        pos->x += vel->x + (pvel?pvel->x:0.0f);  //apply velocity
        pos->y += vel->y + (pvel?pvel->y:0.0f);
        if( pos->x < 0.0f )    //screen edges
          pos->x = 0.0f;
        if( pos->x > 640.0f )
          pos->x = 640.0f;
        if( pos->y>400.0f ) { 
          pos->y = 400.0f;
          vel->y = 0.0f;
        }
      }
    }
    for(r=0;r<10;r++) { //"recurse" up to 10 times to sort out collisions
      memset(recheck[r%2],0,sizeof(recheck[0]));
      for(i=0;i<maxobjs;i++) {
        if(r!=0 && !recheck[(r+1)%2][i])
          continue;
        if(fr[b].objs[i].type!=OBJT_PLAYER)
          continue;
        PLAYER_t *oldme = fr[a].objs[i].data;
        PLAYER_t *newme = fr[b].objs[i].data;
        for(j=0;j<(r<2?i:maxobjs);j++) { //find other players to interact with -- don't need to check all on 1st 2 passes
          if(i==j || fr[b].objs[j].type!=OBJT_PLAYER)
            continue;
          PLAYER_t *oldyou = fr[a].objs[j].data;
          PLAYER_t *newyou = fr[b].objs[j].data;
          if( !oldme || !newme || !oldyou || !newyou ||
              fabsf(newme->pos.x - newyou->pos.x)>20.0f ||  //we dont collide in x NOW
              fabsf(newme->pos.y - newyou->pos.y)>20.0f )   //we dont collide in y NOW
            continue;
          if( oldyou->pos.y - oldme->pos.y >= 20.0f ) {        //I was above BEFORE
            newme->pos.y = newyou->pos.y - 20.0f;
            newme->grounded = 1;
            newme->vel.y = 0.0f;
            recheck[r%2][i] = 1; //I've moved, so recheck me
          } else if( oldme->pos.y - oldyou->pos.y >= 20.0f ) { //You were above BEFORE
            newyou->pos.y = newme->pos.y - 20.0f;
            newyou->grounded = 1;
            newyou->vel.y = 0.0f;
            recheck[r%2][j] = 1; //you've moved, so recheck you
          }
        }
      }
    }
    for(i=0;i<maxobjs;i++) { //mod pass
      OBJ_t *oa = fr[a].objs+i;
      OBJ_t *ob = fr[b].objs+i;
      if(ob->type)
        mod_adv(i,a,b,oa,ob);
    }
    for(i=0;i<maxobjs && creatables>0;i++) { //create dummies if requested
      OBJ_t *ob = fr[b].objs+i;
      if(!ob->type) {
        ob->type = OBJT_DUMMY;
        ob->flags = OBJF_POS|OBJF_VIS;
        ob->size = sizeof(V);
        ob->data = malloc(ob->size);
        V *pos = flex(ob,OBJF_POS);
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
  return -1; //FIXME increase maxobjs instead -- other code will fail until then
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


void *flex(OBJ_t *o,Uint32 part) {
  size_t offset = 0;
  if(!(o->flags & part))
    return NULL;
  TRY
    if(    part == OBJF_POS  ) break;
    if( o->flags & OBJF_POS  ) offset += sizeof(V);
    if(    part == OBJF_VEL  ) break;
    if( o->flags & OBJF_VEL  ) offset += sizeof(V);
    if(    part == OBJF_HULL ) break;
    if( o->flags & OBJF_HULL ) offset += sizeof(V[2]);
    if(    part == OBJF_PVEL ) break;
    if( o->flags & OBJF_PVEL ) offset += sizeof(V);
    return NULL;
  HARDER
  return (V*)(o->data+offset);
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


