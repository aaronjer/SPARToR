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

//file globals
static console_open = 1;


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
  SDL_EnableUNICODE(1);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
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


void input(int press,int sym,Uint16 unicode) {
  Uint32 myfr = hotfr+1;
  if( cmdfr<myfr ) { //this is the new cmdfr, so clear it, unless we already have cmds stored in the future!
                     //TODO: jog the simulation forward if cmds do end up in the future because that must mean we're BEHIND SCHEDULE!
    memset(fr[myfr%maxframes].cmds,0,sizeof(FCMD_t)*maxclients);
    cmdfr = myfr;
  }
  myfr %= maxframes;

  if(press && sym==SDLK_BACKQUOTE)
    toggleconsole();
  else if(press && console_open) {
    if(unicode>31 && unicode<128)
      SJC_Put((char)unicode);
    else if(sym==SDLK_RETURN) {
      if( SJC_Submit() )
        command(SJC.buf[1]);
    }
    else if(sym==SDLK_BACKSPACE)
      SJC_Rub();
    else if(sym==SDLK_ESCAPE && console_open)
      toggleconsole();
  } else switch(sym) {
  case SDLK_LEFT:
    fr[myfr].cmds[me].cmd = press?CMDT_1LEFT:CMDT_0LEFT;
    break;
  case SDLK_RIGHT:
    fr[myfr].cmds[me].cmd = press?CMDT_1RIGHT:CMDT_0RIGHT;
    break;
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


void render() {
  const SDL_VideoInfo *vidinfo;
  SDL_Rect rect;
  int x,y,w,h;
  int i;
  char buf[1000];

  vidinfo = SDL_GetVideoInfo();
  w = vidinfo->current_w;
  h = vidinfo->current_h;

  //display objects
  for(i=0;i<maxobjs;i++) {
    OBJ_t *o = fr[curfr].objs+i;
    if(o->type==OBJT_DUMMY) {
      V *pos = flexpos(o);
      SDL_FillRect(screen,&(SDL_Rect){pos->x-10,pos->y+10,20,20},0xFFFF00);
    }
    if(o->type==OBJT_PLAYER) {
      V *pos = flexpos(o);
      SDL_FillRect(screen,&(SDL_Rect){pos->x-10,pos->y+10,20,20},0x0000FF);
      DrawSquare(screen,&(SDL_Rect){pos->x-10,pos->y+10,20,20},0xFFFFFF);
    }
  }

  //display console
  if(console_open) {
    int conh = h/2 - 40;
    if(conh<40) conh = 40;
    SDL_FillRect(screen,&(SDL_Rect){0,0,w,conh},0x222222);
    x = 10;
    y = conh-20;
    if((ticks/200)%2)
      SJF_DrawChar(screen, x+SJF_TextExtents(SJC.buf[0]), y, '_');
    for(i=0;y>0;i++) {
      if(SJC.buf[i])
        SJF_DrawText(screen,x,y,SJC.buf[i]);
      y -= 10;
    }
  }

  //display ui
  //SJUI_Paint(screen);
  
  //display stats
  sprintf(buf,"fr: meta=%d cur=%d drawn=%d hot=%d",metafr,curfr,drawnfr,hotfr);
  SJF_DrawText(screen,w-20-SJF_TextExtents(buf),h-20,buf);

  SDL_Flip(screen);
  drawnfr = metafr;

  SDL_FillRect(screen,&(SDL_Rect){0,0,w,h},0x000088);
}


void setvideo(int w,int h) {
  screen = SDL_SetVideoMode(w,h,SDL_GetVideoInfo()->vfmt->BitsPerPixel,SDL_RESIZABLE|SDL_DOUBLEBUF);
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



