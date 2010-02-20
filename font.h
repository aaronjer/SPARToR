/**
 **  SuperJer Font
 **
 **  A simple library for drawing a 'system' font on sdl surfaces
 **  which requires no outside resources (images, fonts, etc.)
 **/

#ifndef __SJFONT_H__
#define __SJFONT_H__

#include "SDL.h"

typedef struct
{
  SDL_Surface *surf;
  int w;
  int h;
  int pitch;
  int space[256];
  char raw[128*128];
} SJF_t;

extern SJF_t SJF;

void SJF_Init();
inline void SJF_DrawChar(SDL_Surface *surf, int x, int y, char c);
inline void SJF_DrawText(SDL_Surface *surf, int x, int y, const char *s);
inline int SJF_TextExtents(const char *s);
void SDL_SetPixel(SDL_Surface *surf, int x, int y, Uint8 R, Uint8 G, Uint8 B);

#endif

