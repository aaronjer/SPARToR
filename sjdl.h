/**
 **  SJDL
 **
 **  SuperJer's add-ons for SDL
 **
 **  SJDL_SetPixel() is based on code from the tutorials on libsdl.org
 **/

#ifndef SPARTOR_SJDL_H_
#define SPARTOR_SJDL_H_

#include <GL/glew.h>
#include <GL/gl.h>
#include "SDL.h"

SDL_Surface *SJDL_CopyScaled(SDL_Surface *src, Uint32 flags, int scale);
int SJGL_BlitScaled(GLuint tex, SDL_Rect *s, SDL_Rect *d, int scale, int depth);
int SJDL_BlitScaled(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect, int z);
int SJDL_FillScaled(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color, int scale);
void SJDL_SetPixel(SDL_Surface *surf, int x, int y, Uint8  R, Uint8  G, Uint8  B);
void SJDL_GetPixel(SDL_Surface *surf, int x, int y, Uint8 *R, Uint8 *G, Uint8 *B);
void SJDL_DrawSquare(SDL_Surface *surf, SDL_Rect *rect, unsigned int color);
GLenum SJDL_GLFormatOf(SDL_Surface *surf);

#endif 
