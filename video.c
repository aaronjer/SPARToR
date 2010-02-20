
#include "SDL.h"
#include "SDL_net.h"
#include "video.h"
#include "main.h"
#include "console.h"
#include "font.h"


void render() {
  const SDL_VideoInfo *vidinfo;
  int x,y,w,h;
  int i;
  char buf[1000];
  Uint32 vidfr = (metafr-1);
  Uint32 vidfrmod = vidfr%maxframes;

  Uint32 render_start = SDL_GetTicks();
  static Uint32 unaccounted_start = 0;
  Uint32 tmp;

  Uint32 black  = SDL_MapRGB(screen->format,  0,  0,  0);
  Uint32 white  = SDL_MapRGB(screen->format,255,255,255);
  Uint32 dkgray = SDL_MapRGB(screen->format, 34, 34, 34);
  Uint32 dkblue = SDL_MapRGB(screen->format,  0,  0,136);

  if( metafr==0 || vidfr<=drawnfr ) //==0 prevent never-draw bug
    return;

  vidinfo = SDL_GetVideoInfo();
  w = vidinfo->current_w;
  h = vidinfo->current_h;

  //display objects
  for(i=0;i<maxobjs;i++) {
    OBJ_t *o = fr[vidfrmod].objs+i;
    if(o->type==OBJT_DUMMY) {
      V *pos = flex(o,OBJF_POS);
      SDL_FillRect(screen,&(SDL_Rect){pos->x-10,pos->y-10,20,20},black);
    }
    if(o->type==OBJT_PLAYER) {
      V *pos = flex(o,OBJF_POS);
      Uint32 color = SDL_MapRGB(screen->format, 0x7F*((i/1)%3), 0x7F*((i/3)%3), 0x7F*((i/9)%3));
      SDL_FillRect(screen,&(SDL_Rect){pos->x-10,pos->y-10,20,20},color);
      DrawSquare(screen,&(SDL_Rect){pos->x-10,pos->y-10,20,20},white);
      sprintf(buf,"%d",i);
      SJF_DrawText(screen,pos->x-7,pos->y-8,buf);
    }
  }

  //display console
  if(console_open) {
    int conh = h/2 - 40;
    if(conh<40) conh = 40;
    SDL_FillRect(screen,&(SDL_Rect){0,0,w,conh},dkgray);
    x = 10;
    y = conh-20;
    if((ticks/200)%2)
      SJF_DrawChar(screen, x+SJF_TextExtents(SJC.buf[0]), y, '_');
    for(i=0;y>0;i++) {
      if(SJC.buf[i])
        SJF_DrawText(screen,x,y,SJC.buf[i]);
      y -= 10;
    }
    if( SJC.buf[0] && SJC.buf[0][0] ) {
      char s[10];
      sprintf(s,"%d",SJC.buf[0][strlen(SJC.buf[0])-1]);
      SJF_DrawText(screen,w-20,conh-20,s);
    }
  }

  //display stats
  sprintf(buf,"fr: idx=%d meta=%d vid=%d hot=%d",metafr%maxframes,metafr,vidfr,hotfr);
  SJF_DrawText(screen,w-20-SJF_TextExtents(buf),h-20,buf);

  sprintf(buf,"idle_time %d",idle_time);
  SJF_DrawText(screen,w-20-SJF_TextExtents(buf),h-90,buf);
  sprintf(buf,"render_time %d",render_time);
  SJF_DrawText(screen,w-20-SJF_TextExtents(buf),h-80,buf);
  sprintf(buf,"adv_move_time %d",adv_move_time);
  SJF_DrawText(screen,w-20-SJF_TextExtents(buf),h-70,buf);
  sprintf(buf,"adv_collide_time %d",adv_collide_time);
  SJF_DrawText(screen,w-20-SJF_TextExtents(buf),h-60,buf);
  sprintf(buf,"adv_game_time %d",adv_game_time);
  SJF_DrawText(screen,w-20-SJF_TextExtents(buf),h-50,buf);
  sprintf(buf,"adv_frames %d",adv_frames);
  SJF_DrawText(screen,w-20-SJF_TextExtents(buf),h-40,buf);
  Uint32 accounted_time = idle_time + render_time + adv_move_time + adv_collide_time + adv_game_time;
  Uint32 unaccounted_time = (tmp=SDL_GetTicks()) - unaccounted_start;
  sprintf(buf,"unaccounted_time %d",accounted_time>unaccounted_time?0:unaccounted_time-accounted_time);
  unaccounted_start = tmp;
  SJF_DrawText(screen,w-20-SJF_TextExtents(buf),h-30,buf);

  SDL_Flip(screen);
  SDL_FillRect(screen,&(SDL_Rect){0,0,w,h},dkblue);

  setdrawnfr(vidfr);
  render_time = SDL_GetTicks() - render_start; //keep track of render time
  idle_time = 0;
}


void setvideo(int w,int h) {
  screen = SDL_SetVideoMode(w,h,SDL_GetVideoInfo()->vfmt->BitsPerPixel,SDL_RESIZABLE|SDL_DOUBLEBUF);
}


void DrawSquare(SDL_Surface *surf, SDL_Rect *rect, unsigned int color) {
  SDL_Rect edge;
  edge = *rect;
  edge.w = 1;
  SDL_FillRect(surf,&edge,color);
  edge.x += rect->w - 1;
  SDL_FillRect(surf,&edge,color);
  edge = *rect;
  edge.h = 1;
  SDL_FillRect(surf,&edge,color);
  edge.y += rect->h - 1;
  SDL_FillRect(surf,&edge,color);
}


