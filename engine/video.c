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

#include <GL/glew.h>
#include "SDL.h"
#include "SDL_net.h"
#include "mod.h"
#include "video.h"
#include "main.h"
#include "console.h"
#include "font.h"


TEX_T *textures;
size_t tex_count;


int v_drawhulls  = 0;
int v_showstats  = 0;
int v_usealpha   = 1;
int v_fullscreen = 0;
int v_oob        = 0; // show objects out-of-bounds fading away
int v_center     = 1; // whether to center the scaled game rendering

int v_camx       = 0;
int v_camy       = 0;

int v_w;
int v_h;

static int screen_w  = NATIVEW;
static int screen_h  = NATIVEH;
static int prev_w    = NATIVEW;
static int prev_h    = NATIVEH;
static int desktop_w = 1024;
static int desktop_h = 768;
static int pad_left;
static int pad_top;
static int soon      = 0;
static int soon_w    = 0;
static int soon_h    = 0;
static int soon_full = 0;
static int scale     = 2;


void videoinit()
{
  const SDL_VideoInfo *vidinfo;

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE,    8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,  8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,   8);
  SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE,  8);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
  SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL,1);

  vidinfo = SDL_GetVideoInfo();
  *((int*)&desktop_w) = vidinfo->current_w;
  *((int*)&desktop_h) = vidinfo->current_h;
  int i;
  int default_scale = 1;
  for( i=2; i<=5; i++ )
    if( desktop_w-50 > NATIVEW*i && desktop_h-50 > NATIVEH*i )
      default_scale = i;
  setvideo(NATIVEW*default_scale,NATIVEH*default_scale,0,0);

  GLenum glewerr = glewInit();
  if( glewerr!=GLEW_OK ) { fprintf(stderr,"glewInit: %s\n",glewGetErrorString(glewerr)); exit(-4); }

  SJC_Write("Desktop resolution detected as %d x %d",desktop_w,desktop_h);
  SJC_Write("OpenGL version %s",glGetString(GL_VERSION));
}


void render()
{
  const SDL_VideoInfo *vidinfo;
  int x,y,w,h;
  int i;
  char buf[1000];
  Uint32 vidfr = (metafr-1);
  Uint32 vidfrmod = vidfr%maxframes;

  Uint32 render_start = SDL_GetTicks();
  static Uint32 total_start = 0;
  Uint32 tmp;

  if( metafr==0 || vidfr<=drawnfr ) //==0 prevent never-draw bug
    return;

  if( soon==1 )
    setvideo(soon_w,soon_h,soon_full,0);
  if( soon>0 )
    soon--;

  vidinfo  = SDL_GetVideoInfo();
  w = v_w  = vidinfo->current_w;
  h = v_h  = vidinfo->current_h;
  pad_left = 0;
  pad_top  = 0;
  if( v_center ) {
    pad_left = (w - NATIVEW*scale)/2;
    pad_top  = (h - NATIVEH*scale)/2;
  }

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glScalef(1.0f/256.0f, 1.0f/256.0f, 1);

  glColor4f(1.0f,1.0f,1.0f,1.0f);
  glEnable(GL_TEXTURE_2D);
  if( v_usealpha )
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  else
    glBlendFunc(GL_ONE, GL_ZERO);
  glEnable(GL_BLEND);

  glAlphaFunc(GL_GREATER,0.01);
  glEnable(GL_ALPHA_TEST);

  glDepthFunc(GL_LEQUAL);
  glEnable(GL_DEPTH_TEST);

  glClear(GL_DEPTH_BUFFER_BIT);

  // viewport and matrixes for game objects
  glViewport(pad_left,h-NATIVEH*scale-pad_top,NATIVEW*scale,NATIVEH*scale);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,NATIVEW,NATIVEH,0,-NATIVEH*3-1,NATIVEH*3+1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  int camx = NATIVEW/2-(int)v_camx, camy = NATIVEH/2-(int)v_camy;
  glTranslatef(camx,camy,0);

  SJGL_SetTex( (GLuint)-1 ); //forget previous texture name
  mod_predraw(vidfr);

  //display objects
  for(i=0;i<maxobjs;i++) {
    OBJ_t *o = fr[vidfrmod].objs+i;
    if( o->flags&OBJF_VIS )
      mod_draw(i,o); // have the mod draw the actual thing
  }

  mod_postdraw(vidfr);

  glDisable(GL_DEPTH_TEST);

  //display hulls and object numbers
  if( v_drawhulls ) {
    glBindTexture( GL_TEXTURE_2D, 0 );
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    if( mycontext ) {
      CONTEXT_t *co = fr[vidfrmod].objs[mycontext].data;
      int x,y,z;
      for( z=0; z<co->z; z++ )
        for( y=0; y<co->y; y++ )
          for( x=0; x<co->x; x++ ) {
            int pos = co->x*co->y*z + co->x*y + x;
            int flags;

            if( co->dmap[ pos ].flags & CBF_NULL )
              flags = co->map[  pos ].flags;
            else
              flags = co->dmap[ pos ].flags;

            if( flags & CBF_SOLID ) {
              glColor4f(1,0,0,1);
              SJGL_Blit( &(SDL_Rect){0,0,16,16}, x*16,   y*16,   z );
              SJGL_Blit( &(SDL_Rect){0,0,12,12}, x*16+2, y*16+2, z );
            } else if( flags & CBF_PLAT ) {
              glColor4f(0,1,0,1);
              SJGL_Blit( &(SDL_Rect){0,0,16, 2}, x*16,   y*16,   z );
            }
          }
    }

    glColor4f(1,1,1,1);
    for(i=0;i<maxobjs;i++) {
      OBJ_t *o = fr[vidfrmod].objs+i;
      V *pos  = flex(o,OBJF_POS);
      V *hull = flex(o,OBJF_HULL);
      if( pos && hull ) {
        SDL_Rect rect = (SDL_Rect){0, 0, hull[1].x-hull[0].x, hull[1].y-hull[0].y};
        SJGL_Blit( &rect, pos->x+hull[0].x, pos->y+hull[0].y, 0 );
      }
    }

    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

    for(i=0;i<maxobjs;i++) {
      OBJ_t *o = fr[vidfrmod].objs+i;
      V *pos  = flex(o,OBJF_POS);
      if( pos ) {
        sprintf(buf,"%d",i);
        SJF_DrawText(pos->x, pos->y, buf);
      }
    }
  }

  // viewport and matrixes for HUD
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,w,h,0,-1,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  //paint black over the border areas, subtractively with v_oob
  {
    int outerl = 0;   int innerl = pad_left;
    int outert = 0;   int innert = pad_top;
    int outerr = w;   int innerr = pad_left + NATIVEW*scale;
    int outerb = h;   int innerb = pad_top  + NATIVEH*scale;
    glDisable(GL_TEXTURE_2D);
    glPushAttrib(GL_COLOR_BUFFER_BIT);
    if( v_oob ) {
      glColor4f(0.02,0.02,0.02,0.02);
      glBlendFunc(GL_ONE,GL_ONE);
      if( GLEW_EXT_blend_equation_separate )
        glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
    }
    else
      glColor4f(0,0,0,1.0f);
    glBegin(GL_QUADS);
    glVertex2i(outerl,outert); glVertex2i(outerr,outert); glVertex2i(outerr,innert); glVertex2i(outerl,innert); //top
    glVertex2i(outerl,innerb); glVertex2i(outerr,innerb); glVertex2i(outerr,outerb); glVertex2i(outerl,outerb); //bottom
    glVertex2i(outerl,innert); glVertex2i(innerl,innert); glVertex2i(innerl,innerb); glVertex2i(outerl,innerb); //left
    glVertex2i(innerr,innert); glVertex2i(outerr,innert); glVertex2i(outerr,innerb); glVertex2i(innerr,innerb); //right
    glEnd();
    glPopAttrib();
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f,1.0f,1.0f,1.0f);
  }

  SJGL_SetTex( (GLuint)-1 ); //forget previous texture name
  mod_outerdraw(vidfr,w,h);

  //display console
  if(console_open) {
    int conh = h/2 - 40;
    if(conh<40) conh = 40;
    glColor4f(0.15,0.15,0.15,0.85);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glVertex2i(0,   0); glVertex2i(w,   0);
    glVertex2i(w,conh); glVertex2i(0,conh);
    glEnd();
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0f,1.0f,1.0f,1.0f);
    x = 10;
    y = conh-20;
    if((ticks/200)%2)
      SJF_DrawChar(x+SJF_TextExtents(SJC.buf[0]), y, '_');
    for(i=0;y>0;i++) {
      if(SJC.buf[i])
        SJF_DrawText(x,y,SJC.buf[i]);
      y -= 10;
    }
    if( SJC.buf[0] && SJC.buf[0][0] ) {
      char s[10];
      sprintf(s,"%d",SJC.buf[0][strlen(SJC.buf[0])-1]);
      SJF_DrawText(w-20,conh-20,s);
    }
  }

  //display stats
  total_time += (tmp = SDL_GetTicks()) - total_start;
  render_time += tmp - render_start;
  total_start = tmp;
  Uint32 unaccounted_time = total_time - (idle_time + render_time + adv_move_time + adv_collide_time + adv_game_time);
  if( v_showstats ) {
    Uint32 denom = vidfrmod+1;
    sprintf(buf,"idle_time %4d"       ,       idle_time/denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf),10,buf);
    sprintf(buf,"render_time %4d"     ,     render_time/denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf),20,buf);
    sprintf(buf,"adv_move_time %4d"   ,   adv_move_time/denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf),30,buf);
    sprintf(buf,"adv_collide_time %4d",adv_collide_time/denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf),40,buf);
    sprintf(buf,"adv_game_time %4d"   ,   adv_game_time/denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf),50,buf);
    sprintf(buf,"unaccounted_time %4d",unaccounted_time/denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf),60,buf);
    sprintf(buf,"adv_frames  %2.2f"   ,(float)adv_frames/(float)denom);
    SJF_DrawText(w-20-SJF_TextExtents(buf),70,buf);
    sprintf(buf,"fr: idx=%d meta=%d vid=%d hot=%d",metafr%maxframes,metafr,vidfr,hotfr);
    SJF_DrawText(w-20-SJF_TextExtents(buf),80,buf);
  }

  SDL_GL_SwapBuffers();
  setdrawnfr(vidfr);

  if( (int)vidfrmod==maxframes-1 ) { // reset time stats
    total_time       = 0;
    idle_time        = 0;
    render_time      = 0;
    adv_move_time    = 0;
    adv_collide_time = 0;
    adv_game_time    = 0;
    adv_frames       = 0;
  }
}


void setvideo(int w,int h,int go_full,int quiet)
{
  Uint32 flags = 0;
  if( !w || !h ) { //default to previous res
    w = prev_w;
    h = prev_h;
  }
  if( go_full && !v_fullscreen ) { // record previous res when changing to fullscreen
    prev_w = screen_w;
    prev_h = screen_h;
  }
  v_fullscreen = go_full;
  flags |= (go_full ? SDL_FULLSCREEN : SDL_RESIZABLE);
  screen = SDL_SetVideoMode(w,h,SDL_GetVideoInfo()->vfmt->BitsPerPixel,SDL_OPENGL|flags);
  if( !screen ){
    v_fullscreen = 0;
    screen = SDL_SetVideoMode(NATIVEW,NATIVEH,SDL_GetVideoInfo()->vfmt->BitsPerPixel,SDL_OPENGL|SDL_RESIZABLE);
    SJC_Write("Error changing video mode. Using safe defaults.");
    if( !screen ) {
      fprintf(stderr,"Fatal error setting video mode!");
      exit(-4);
    }
  }
  const SDL_VideoInfo *vidinfo = SDL_GetVideoInfo();
  screen_w = w = vidinfo->current_w;
  screen_h = h = vidinfo->current_h;
  scale = (h/NATIVEH < w/NATIVEW) ? h/NATIVEH : w/NATIVEW;
  if( scale<1 )
    scale = 1;
  SJF_Init();
  mod_setvideo(w,h);
  if( !quiet )
    SJC_Write("Video mode set to %d x %d",w,h);
}


void setvideosoon(int w,int h,int go_full,int delay)
{
  if( !w || !h ) {
    if( go_full ) {
      soon_w = desktop_w;
      soon_h = desktop_h;
    } else {
      soon_w = prev_w;
      soon_h = prev_h;
    }
  } else {
    soon_w = w;
    soon_h = h;
  }
  soon_full = go_full;
  soon = delay;
}


int screen2native_x(int x)
{
  return (x - pad_left)/scale;
}


int screen2native_y(int y)
{
  return (y - pad_top)/scale;
}


int make_sure_texture_is_loaded(const char *texfile)
{
  SDL_Surface *surf;
  size_t j,k;

  glPixelStorei(GL_UNPACK_ALIGNMENT,4);

  for( j=0; ; j++ ) {
    // need more space?
    if( j >= tex_count ) {
      size_t new_count = tex_count < 8 ? 8 : tex_count*2;
      textures = realloc( textures, new_count * sizeof *textures );
      memset( textures + tex_count, 0, (new_count - tex_count) * sizeof *textures );
      tex_count = new_count;
    }

    // is slot j the one?
    if( !textures[j].filename ) {
      SJC_Write("Texture %s is new, using slot %d",texfile,j);
      textures[j].filename = malloc( strlen(texfile) + 1 );
      strcpy(textures[j].filename,texfile);
    } else if( !strcmp(textures[j].filename,texfile) ) {
      SJC_Write("Texture %s was loaded before, reusing slot %d",texfile,j);
      if( textures[j].generated )
        return j; // nothing to do!
    } else
      continue;

    // alert GL to presence of new texture
    glGenTextures(1,&textures[j].glname);
    textures[j].generated = 1;

    // load texture into GL
    glBindTexture(GL_TEXTURE_2D,textures[j].glname);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    if( (surf = IMG_Load(textures[j].filename)) ) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surf->w, surf->h, 0, SJDL_GLFormatOf(surf), GL_UNSIGNED_BYTE, surf->pixels);
      SDL_FreeSurface(surf);
    } else
      SJC_Write("Failed to read texture %s",textures[j].filename);

    // map system texture if there is a match(es)
    for( k=0; k<num_sys_tex; k++ )
      if( strstr( textures[j].filename, sys_tex[k].name ) )
        sys_tex[k].num = j;

    return j;
  }
}


