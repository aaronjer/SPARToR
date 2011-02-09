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
#include <GL/gl.h>
#include "SDL.h"
#include "sjdl.h"

//create a new surface by copying and scaling another
SDL_Surface *SJDL_CopyScaled(SDL_Surface *src,Uint32 flags,int scale) {
  Uint32 u,v,i,j;
  Uint8 r,g,b;
  SDL_Surface *dst = SDL_CreateRGBSurface(flags,src->w*scale,src->h*scale,
                                          src->format->BitsPerPixel,
                                          src->format->Rmask,
                                          src->format->Gmask,
                                          src->format->Bmask,
                                          src->format->Amask);
  SDL_LockSurface(src);
  SDL_LockSurface(dst);
  for(u=0; u<src->w; u++)
    for(v=0; v<src->h; v++) {
      SJDL_GetPixel(src, u, v, &r, &g, &b);
      for(i=0; i<scale; i++)
        for(j=0; j<scale; j++)
          SJDL_SetPixel(dst, u*scale+i, v*scale+j,  r,  g,  b);
    }
  SDL_UnlockSurface(dst);
  SDL_UnlockSurface(src);
  return dst;
}

//uses GL to do the same thing as SJDL_BlitScaled
int SJGL_BlitScaled(GLuint tex, SDL_Rect *s, SDL_Rect *d, int scale, int z) {
  glBindTexture(GL_TEXTURE_2D,0); //FIXME: hack 4 win
  glBindTexture(GL_TEXTURE_2D,tex);

  if( z<0 ) z = (d->y+s->h)*-z;

  d->x *= scale;
  d->y *= scale;
  d->w = s->w*scale;
  d->h = s->h*scale;

  glBegin(GL_QUADS);
  glTexCoord3i(s->x     ,s->y     ,z); glVertex3i(d->x     ,d->y     ,z);
  glTexCoord3i(s->x+s->w,s->y     ,z); glVertex3i(d->x+d->w,d->y     ,z);
  glTexCoord3i(s->x+s->w,s->y+s->h,z); glVertex3i(d->x+d->w,d->y+d->h,z);
  glTexCoord3i(s->x     ,s->y+s->h,z); glVertex3i(d->x     ,d->y+d->h,z);
  glEnd();

  return 0;
}

//blits from one surface to another after scaling s/drect positions and sizes
int SJDL_BlitScaled(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect, int scale) {
  SDL_Rect srcrect2 = (SDL_Rect){srcrect->x*scale, srcrect->y*scale, srcrect->w*scale, srcrect->h*scale};
  dstrect->x *= scale;
  dstrect->y *= scale;
  return SDL_BlitSurface(src,&srcrect2,dst,dstrect);
}

//fills a rectangle after scaling its position and size
int SJDL_FillScaled(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color, int scale) {
  SDL_Rect dstrect2 = (SDL_Rect){dstrect->x*scale, dstrect->y*scale, dstrect->w*scale, dstrect->h*scale};
  return SDL_FillRect(dst,&dstrect2,color);
}

//sets a pixel on an sdl surface
void SJDL_SetPixel(SDL_Surface *surf, int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
  Uint32 color = SDL_MapRGB(surf->format, R, G, B);
  switch(surf->format->BytesPerPixel) {
  case 1: // 8-bpp
    {
      Uint8 *bufp;
      bufp = (Uint8 *)surf->pixels + y*surf->pitch + x;
      *bufp = color;
    }
    break;
  case 2: // 15-bpp or 16-bpp
    {
      Uint16 *bufp;
      bufp = (Uint16 *)surf->pixels + y*surf->pitch/2 + x;
      *bufp = color;
    }
    break;
  case 3: // 24-bpp mode, usually not used
    {
      Uint8 *bufp;
      bufp = (Uint8 *)surf->pixels + y*surf->pitch + x * 3;
      if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
      {
        bufp[0] = color;
        bufp[1] = color >> 8;
        bufp[2] = color >> 16;
      } else {
        bufp[2] = color;
        bufp[1] = color >> 8;
        bufp[0] = color >> 16;
      }
    }
    break;
  case 4: // 32-bpp
    {
      Uint32 *bufp;
      bufp = (Uint32 *)surf->pixels + y*surf->pitch/4 + x;
      *bufp = color;
    }
    break;
  }
} 


//gets a pixel on an sdl surface
void SJDL_GetPixel(SDL_Surface *surf, int x, int y, Uint8 *R, Uint8 *G, Uint8 *B)
{
  Uint32 color;
  switch(surf->format->BytesPerPixel) {
  case 1: // 8-bpp
    {
      Uint8 *bufp;
      bufp = (Uint8 *)surf->pixels + y*surf->pitch + x;
      color = *bufp;
    }
    break;
  case 2: // 15-bpp or 16-bpp
    {
      Uint16 *bufp;
      bufp = (Uint16 *)surf->pixels + y*surf->pitch/2 + x;
      color = *bufp;
    }
    break;
  case 3: // 24-bpp mode, usually not used
    {
      Uint8 *bufp;
      bufp = (Uint8 *)surf->pixels + y*surf->pitch + x * 3;
      if(SDL_BYTEORDER == SDL_LIL_ENDIAN)
        color  = bufp[0] | (bufp[1] << 8) | (bufp[2] << 16);
      else
        color  = bufp[2] | (bufp[1] << 8) | (bufp[0] << 16);
    }
    break;
  case 4: // 32-bpp
    {
      Uint32 *bufp;
      bufp = (Uint32 *)surf->pixels + y*surf->pitch/4 + x;
      color = *bufp;
    }
    break;
  }
  SDL_GetRGB(color, surf->format, R, G, B);
} 


void SJDL_DrawSquare(SDL_Surface *surf, SDL_Rect *rect, unsigned int color) {
  SDL_Rect edge;
  glBegin(GL_LINE_LOOP);
  glVertex2i(rect->x        ,rect->y        );
  glVertex2i(rect->x+rect->w,rect->y        );
  glVertex2i(rect->x+rect->w,rect->y+rect->h);
  glVertex2i(rect->x        ,rect->y+rect->h);
  glEnd();
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


// returns the equivalent opengl format of an sdl surface
GLenum SJDL_GLFormatOf(SDL_Surface *surf) {
  if( surf->format->Amask > 0 ) {
    if( surf->format->Rmask > surf->format->Bmask )
      return (SDL_BYTEORDER==SDL_LIL_ENDIAN ? GL_BGRA : GL_RGBA);
    else
      return (SDL_BYTEORDER==SDL_LIL_ENDIAN ? GL_RGBA : GL_BGRA);
  } else {
    if( surf->format->Rmask > surf->format->Bmask )
      return (SDL_BYTEORDER==SDL_LIL_ENDIAN ? GL_BGR : GL_RGB);
    else
      return (SDL_BYTEORDER==SDL_LIL_ENDIAN ? GL_RGB : GL_BGR);
  }
}


