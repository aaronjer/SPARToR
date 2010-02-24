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
#include "video.h"
#include "main.h"
#include "console.h"
#include "font.h"
#include "mod.h"

int drawhulls = 0;
int showstats = 1;
int fullscreen = 0;
int video_reset = 0;



void render() {
  const SDL_VideoInfo *vidinfo;
  int x,y,w,h;
  int i;
  char buf[1000];
  Uint32 vidfr = (metafr-1);
  Uint32 vidfrmod = vidfr%maxframes;

  Uint32 render_start = SDL_GetTicks();
  static Uint32 total_start = 0;
  Uint32 tmp;

  Uint32 dkgray = SDL_MapRGB(screen->format,0x22,0x22,0x22);
  Uint32 color;

  if( metafr==0 || vidfr<=drawnfr ) //==0 prevent never-draw bug
    return;

  vidinfo = SDL_GetVideoInfo();
  w = vidinfo->current_w;
  h = vidinfo->current_h;

  if( video_reset ) {
    setvideo(w,h,0);
    video_reset = 0;
  }

  mod_predraw(screen,vidfr);

  //display objects
  for(i=0;i<maxobjs;i++) {
    OBJ_t *o = fr[vidfrmod].objs+i;
    V *pos  = flex(o,OBJF_POS);
    V *hull = flex(o,OBJF_HULL);

    if( o->flags&OBJF_VIS )
      mod_draw(screen,i,o); // have the mod draw the actual thing

    if( pos && hull && drawhulls ) {
      SDL_Rect rect;
      rect = (SDL_Rect){pos->x+hull[0].x, pos->y+hull[0].y, hull[1].x-hull[0].x, hull[1].y-hull[0].y};
      color = SDL_MapRGB(screen->format, 0x90+0x22*((i/1)%3), 0x90+0x22*((i/3)%3), 0x90+0x22*((i/9)%3));
      rect.x *= scale;
      rect.y *= scale;
      rect.w *= scale;
      rect.h *= scale;
      SJDL_DrawSquare(screen,&rect,color);
    }
    if( pos && drawhulls ) {
      sprintf(buf,"%d",i);
      SJF_DrawText(screen, pos->x*scale, pos->y*scale, buf);
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
  total_time += (tmp = SDL_GetTicks()) - total_start;
  render_time += tmp - render_start;
  total_start = tmp;
  Uint32 unaccounted_time = total_time - (idle_time + render_time + adv_move_time + adv_collide_time + adv_game_time);
  if( showstats ) {
    Uint32 denom = vidfrmod+1;
    sprintf(buf,"idle_time %4d"       ,       idle_time/denom);
    SJF_DrawText(screen,w-20-SJF_TextExtents(buf),10,buf);
    sprintf(buf,"render_time %4d"     ,     render_time/denom);
    SJF_DrawText(screen,w-20-SJF_TextExtents(buf),20,buf);
    sprintf(buf,"adv_move_time %4d"   ,   adv_move_time/denom);
    SJF_DrawText(screen,w-20-SJF_TextExtents(buf),30,buf);
    sprintf(buf,"adv_collide_time %4d",adv_collide_time/denom);
    SJF_DrawText(screen,w-20-SJF_TextExtents(buf),40,buf);
    sprintf(buf,"adv_game_time %4d"   ,   adv_game_time/denom);
    SJF_DrawText(screen,w-20-SJF_TextExtents(buf),50,buf);
    sprintf(buf,"unaccounted_time %4d",unaccounted_time/denom);
    SJF_DrawText(screen,w-20-SJF_TextExtents(buf),60,buf);
    sprintf(buf,"adv_frames  %2.2f"   ,(float)adv_frames/(float)denom);
    SJF_DrawText(screen,w-20-SJF_TextExtents(buf),70,buf);
    sprintf(buf,"fr: idx=%d meta=%d vid=%d hot=%d",metafr%maxframes,metafr,vidfr,hotfr);
    SJF_DrawText(screen,w-20-SJF_TextExtents(buf),80,buf);
  }

  SDL_Flip(screen);
  setdrawnfr(vidfr);

  if( vidfrmod==maxframes-1 ) { // reset time stats
    total_time       = 0;
    idle_time        = 0;
    render_time      = 0;
    adv_move_time    = 0;
    adv_collide_time = 0;
    adv_game_time    = 0;
    adv_frames       = 0;
  }
}


void setvideo(int w,int h,Uint32 flags) {
  if( w/384 > h/240 )
    scale = h/240;
  else
    scale = w/384;
  if( scale<1 )
    scale = 1;
  flags |= SDL_RESIZABLE|SDL_DOUBLEBUF|SDL_HWSURFACE|SDL_ANYFORMAT;
  screen = SDL_SetVideoMode(w,h,SDL_GetVideoInfo()->vfmt->BitsPerPixel,flags);
  mod_setvideo(w,h);
}



