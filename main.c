/**
 **  SPARToR [CORE]
 **  A.K.A SPARToR 4
 **
 **  100% SDL+OpenGL now
 **  Network-CORE-centered
 **
 **/


#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
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


//globals
size_t maxframes = 360;
size_t maxobjs = 100;

FRAME_t *fr;
Uint32 metafr;
Uint32 curfr;
Uint32 drawnfr;
Uint32 hotfr;
Uint32 cmdfr = 1; //DO NOT clear frame 1, it is prefilled with client-connect for local person
int creatables;

SDL_Surface *screen;
Uint32 ticks;
int me;
int console_open;

//file globals


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
  toggleconsole();
  setvideo(640,480);
  SDL_WM_SetCaption("SPARToR CORE",NULL);
  vidinfo = SDL_GetVideoInfo();

  SJF_Init();

  //UI
  SJUI_Init();
  SJUI_HANDLE h = SJUI_NewControl(0,50,90,0);
  SJUI_SetPos(h,10,10);

  SJC_Write("SPARToR CORE v%s",VERSION);

  //main loop
  while(1) {
    ticks = SDL_GetTicks();
    metafr = ticks/40;
    curfr = metafr%maxframes;
    while( SDL_PollEvent(&event) ) switch(event.type) {
      case SDL_VIDEOEXPOSE:
        break;
      case SDL_VIDEORESIZE:
        setvideo(event.resize.w,event.resize.h);
        break;
      case SDL_KEYDOWN:
        input( 1, event.key.keysym.sym, event.key.keysym.unicode );
        break;
      case SDL_KEYUP:
        input( 0, event.key.keysym.sym, event.key.keysym.unicode );
        break;
      case SDL_QUIT:
        cleanup();
    }
    readinput();
    if(hostsock) host();
    if(clientsock) client();
    advance();
    if(metafr!=drawnfr)
      render();
    //SDL_Delay(10);
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
  while(hotfr < metafr) {
    Uint32 a = (hotfr+0)%maxframes;
    Uint32 b = (hotfr+1)%maxframes;
    Uint32 c = (hotfr+2)%maxframes;
    if( cmdfr<hotfr+1 ) { //need to clear out the cmds in the frame since it hasn't been done yet!
      memset(fr[b].cmds,0,sizeof(FCMD_t)*maxclients);
      cmdfr = hotfr+1;
    }
    for(i=0;i<maxobjs;i++) { //advance each object into the hot fresh frame
      OBJ_t *oa = fr[a].objs+i;
      OBJ_t *ob = fr[b].objs+i;
      OBJ_t *oc = fr[c].objs+i;
      oc->type = 0;
      free(oc->data);
      if(oa->type) {
        ob->type = oa->type;
        ob->flags = oa->flags;
        ob->size = oa->size;
        ob->data = malloc(oa->size);
        memcpy(ob->data,oa->data,oa->size);
        if(ob->type==OBJT_DUMMY) {
          V *pos = flexpos(ob);
          pos->x += (float)(metafr%50) - 24.5f;
        }
        mod_adv(a,b,oa,ob);
      } else 
        ; //nothing to do since this was cleared out last iteration
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
    hotfr++;
  }
}


void cleanup() {
  int i;
  SJUI_Destroy();
  SDLNet_Quit();
  SDL_Quit();
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
  while(last_slot<maxobjs) {
    if( fr[frame0].objs[last_slot].type==0 ) //empty
      return last_slot++;
    last_slot++;
  }
  return -1;
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



