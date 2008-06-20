
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "video.h"
#include "main.h"
#include "console.h"
#include "font.h"
#include "sjui.h"


void render() {
  const SDL_VideoInfo *vidinfo;
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


