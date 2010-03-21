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
#include "SDL_opengl.h"
#include "video.h"
#include "main.h"
#include "console.h"
#include "font.h"
#include "mod.h"

int drawhulls = 0;
int showstats = 1;
int usealpha = 1;
int fullscreen = 0;
int screen_w = NATIVEW;
int screen_h = NATIVEH;
int prev_w = NATIVEW;
int prev_h = NATIVEH;
int desktop_w = 1024;
int desktop_h = 768;
int soon = 1;
int soon_w = 0;
int soon_h = 0;

static int pad_left;
static int pad_top;



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

  Uint32 color;

  if( metafr==0 || vidfr<=drawnfr ) //==0 prevent never-draw bug
    return;

  if( soon==1 )
    setvideo(soon_w,soon_h,0);
  if( soon>0 )
    soon--;

  vidinfo = SDL_GetVideoInfo();
  w = vidinfo->current_w;
  h = vidinfo->current_h;

  glViewport(0,0,w,h);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(1.0f/256.0f, 1.0f/256.0f, 1);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  pad_left = (w - NATIVEW*scale)/2;
  pad_top  = (h - NATIVEH*scale)/2;
  glOrtho(-pad_left,w-pad_left,h-pad_top,-pad_top,-NATIVEH*3-1,NATIVEH*3+1);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glColor4f(1.0f,1.0f,1.0f,1.0f);
  glEnable(GL_TEXTURE_2D);
  if( usealpha )
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  else
    glBlendFunc(GL_ONE, GL_ZERO);
  glEnable(GL_BLEND);

  glAlphaFunc(GL_GREATER,0.01);
  glEnable(GL_ALPHA_TEST);

  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);

  glClear(GL_DEPTH_BUFFER_BIT);

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
      SJF_DrawText(pos->x*scale, pos->y*scale, buf);
    }
  }

  //paint over border areas just a lil bit
  {
    float outerl =  -pad_left;   float innerl = 0;
    float outert =  -pad_top;    float innert = 0;
    float outerr = w-pad_left;   float innerr = NATIVEW*scale;
    float outerb = h-pad_top;    float innerb = NATIVEH*scale;
    glColor4f(0.0,0.0,0.0,0.1);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glVertex2f(outerl,outert); glVertex2f(outerr,outert); glVertex2f(outerr,innert); glVertex2f(outerl,innert); //top
    glVertex2f(outerl,innerb); glVertex2f(outerr,innerb); glVertex2f(outerr,outerb); glVertex2f(outerl,outerb); //bottom
    glVertex2f(outerl,innert); glVertex2f(innerl,innert); glVertex2f(innerl,innerb); glVertex2f(outerl,innerb); //left
    glVertex2f(innerr,innert); glVertex2f(outerr,innert); glVertex2f(outerr,innerb); glVertex2f(innerr,innerb); //right
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f,1.0f,1.0f,1.0f);
  }

  //display console
  if(console_open) {
    int conh = h/2 - 40;
    if(conh<40) conh = 40;
    glDepthFunc(GL_ALWAYS);
    glColor4f(0.15,0.15,0.15,0.85);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glVertex2f( -pad_left,    -pad_top); glVertex2f(w-pad_left,    -pad_top);
    glVertex2f(w-pad_left,conh-pad_top); glVertex2f( -pad_left,conh-pad_top);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    x = 10;
    y = conh-20;
    if((ticks/200)%2)
      SJF_DrawChar(x+SJF_TextExtents(SJC.buf[0])-pad_left, y-pad_top, '_');
    for(i=0;y>0;i++) {
      if(SJC.buf[i])
        SJF_DrawText(x-pad_left,y-pad_top,SJC.buf[i]);
      y -= 10;
    }
    if( SJC.buf[0] && SJC.buf[0][0] ) {
      char s[10];
      sprintf(s,"%d",SJC.buf[0][strlen(SJC.buf[0])-1]);
      SJF_DrawText(w-20-pad_left,conh-20-pad_top,s);
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
    SJF_DrawText(w-20-SJF_TextExtents(buf)-pad_left,10-pad_top,buf);
    sprintf(buf,"render_time %4d"     ,     render_time/denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf)-pad_left,20-pad_top,buf);
    sprintf(buf,"adv_move_time %4d"   ,   adv_move_time/denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf)-pad_left,30-pad_top,buf);
    sprintf(buf,"adv_collide_time %4d",adv_collide_time/denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf)-pad_left,40-pad_top,buf);
    sprintf(buf,"adv_game_time %4d"   ,   adv_game_time/denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf)-pad_left,50-pad_top,buf);
    sprintf(buf,"unaccounted_time %4d",unaccounted_time/denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf)-pad_left,60-pad_top,buf);
    sprintf(buf,"adv_frames  %2.2f"   ,(float)adv_frames/(float)denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf)-pad_left,70-pad_top,buf);
    sprintf(buf,"fr: idx=%d meta=%d vid=%d hot=%d",metafr%maxframes,metafr,vidfr,hotfr);
    SJF_DrawText(w-20-SJF_TextExtents(buf)-pad_left,80-pad_top,buf);
  }

  SDL_GL_SwapBuffers();
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


void setvideo(int w,int h,int quiet) {
  Uint32 flags = SDL_OPENGL;
  if( fullscreen )
    flags |= SDL_FULLSCREEN;
  else
    flags |= SDL_RESIZABLE;
  screen = SDL_SetVideoMode(w,h,SDL_GetVideoInfo()->vfmt->BitsPerPixel,flags);
  if( !screen ){
    screen = SDL_SetVideoMode(NATIVEW,NATIVEH,SDL_GetVideoInfo()->vfmt->BitsPerPixel,0);
    SJC_Write("Error changing video mode. Using safe defaults.");
    if( !screen ) {
      fprintf(stderr,"Fatal error setting video mode!");
      exit(-4);
    }
  }
  const SDL_VideoInfo *vidinfo = SDL_GetVideoInfo();
  w = vidinfo->current_w;
  h = vidinfo->current_h;
  scale = (w/NATIVEW > h/NATIVEH) ? h/NATIVEH : w/NATIVEW;
  if( scale<1 )
    scale = 1;
  SJF_Init();
  mod_setvideo(w,h);
  if( !quiet )
    SJC_Write("Video mode set to %d x %d",w,h);
}

void setvideosoon(int w,int h,int delay) {
  soon_w = w;
  soon_h = h;
  soon = delay;
}

