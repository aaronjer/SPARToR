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


//globals
size_t maxframes = 360;
size_t maxobjs = 100;

FRAME_t *fr;
Uint32 metafr;
Uint32 curfr;
Uint32 drawnfr;
Uint32 hotfr;
int creatables;

SDL_Surface *screen;
Uint32 ticks;

//file globals
static console_open = 1;


int main(int argc,char **argv) {
  SDL_Event event;
  const SDL_VideoInfo *vidinfo;
  int sym;
  int i;

  fr = calloc(sizeof(FRAME_t),maxframes);
  for(i=0;i<maxframes;i++) {
    fr[i].cmds = calloc(sizeof(FCMD_t),maxclients);
    fr[i].objs = calloc(sizeof(OBJ_t),maxobjs);
  }

  if( SDL_Init(SDL_INIT_TIMER|SDL_INIT_AUDIO|SDL_INIT_VIDEO)<0 || !SDL_GetVideoInfo() ) {
    fprintf(stderr,"SDL_Init: %s\n",SDL_GetError());
    exit(-1);
  }
  if( SDLNet_Init()<0 ) {
    fprintf(stderr,"SDLNet_Init: %s\n",SDL_GetError());
    exit(-2);
  }
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
    metafr = ticks/50;
    curfr = metafr%maxframes;
    while( SDL_PollEvent(&event) ) switch(event.type) {
      case SDL_VIDEOEXPOSE:
        break;
      case SDL_VIDEORESIZE:
        setvideo(event.resize.w,event.resize.h);
        break;
      case SDL_KEYDOWN:
        sym = event.key.keysym.sym;
        switch(sym) {
          case SDLK_ESCAPE:
            if(console_open) console_open=0;
            else cleanup();
            break;
          case SDLK_BACKQUOTE:
            console_open = !console_open;
            break;
          default:
            if(console_open) {
              if(sym>31 && sym<128)
                SJC_Put((char)sym);
              else if(sym==SDLK_RETURN) {
                if( SJC_Submit() )
                  command(SJC.buf[1]);
              }
              else if(sym==SDLK_BACKSPACE)
                SJC_Rub();
            }
        }
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


void advance() {
  int i;
  int adv_nul = 0;
  int adv_cpy = 0;
  while(hotfr < metafr) {
    Uint32 a = (hotfr+0)%maxframes;
    Uint32 b = (hotfr+1)%maxframes;
    for(i=0;i<maxobjs;i++) {
      OBJ_t *oa = fr[a].objs+i;
      OBJ_t *ob = fr[b].objs+i;
      free(ob->data);
      if(oa->type) {
        ob->type = oa->type;
        ob->flags = oa->flags;
        ob->size = oa->size;
        ob->data = malloc(oa->size);
        memcpy(ob->data,oa->data,oa->size);
        if(ob->flags & OBJF_POS){
          V *pos = flexpos(ob);
          pos->x += (float)(metafr%50) - 25.0f;
        }
        adv_cpy++;
      } else {
        ob->type = 0;
        ob->data = NULL;
        adv_nul++;
      }
    }
    for(i=0;i<maxobjs && creatables>0;i++) {
      OBJ_t *ob = fr[b].objs+i;
      if(!ob->type) {
        ob->type = 1;
        ob->flags = OBJF_POS|OBJF_VIS;
        ob->size = sizeof(V);
        ob->data = malloc(ob->size);
        V *pos = flexpos(ob);
        *pos = (V){320.0f,280.0f,0.0f};
        creatables--;
        SJC_Write("Created new OBJ_t at frame %d, obj %d, addr %X",b,i,&ob);
      }
    }
    hotfr++;
  }
  //if( adv_nul || adv_cpy )
  //  SJC_Write("advance(): %d nulled, %d copied",adv_nul,adv_cpy);
}


void render() {
  const SDL_VideoInfo *vidinfo;
  SDL_Rect rect;
  Uint32 x,y,w,h;
  int i;
  char buf[1000];

  vidinfo = SDL_GetVideoInfo();
  w = vidinfo->current_w;
  h = vidinfo->current_h;

  //display objects
  for(i=0;i<maxobjs;i++) {
    OBJ_t *o = fr[curfr].objs+i;
    if(o->type!=0) {
      V *pos = flexpos(o);
      SDL_FillRect(screen,&(SDL_Rect){pos->x-10,pos->y+10,20,20},0xFFFF00);
    }
  }

  //display console
  if(console_open) {
    SDL_FillRect(screen,&(SDL_Rect){0,0,w,200},0x222222);
    x = 10;
    y = 200-20;
    if((ticks/200)%2)
      SJF_DrawChar(screen, x+SJF_TextExtents(SJC.buf[0]), y, '_');
    for(i=0;i<18;i++) {
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


V *flexpos(OBJ_t *o){
   if(!(o->flags & OBJF_POS))
     return NULL;
   return (V*)(o->data);
}



