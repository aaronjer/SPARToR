/**
 **  SuperJer Font
 **
 **  A simple library for drawing a 'system' font on SDL GL surfaces
 **  which requires no outside resources (images, fonts, etc.)
 **/

#ifndef SPARTOR_SJFONT_H_
#define SPARTOR_SJFONT_H_

#include "SDL.h"
#include "SDL_opengl.h"
#include "sjdl.h"

typedef struct
{
  GLuint tex;
  int w;
  int h;
  int pitch;
  int space[256];
  char raw[128*128];
} SJF_t;

extern SJF_t SJF;

void SJF_Init();
void SJF_DrawChar(int x, int y, char ch);
void SJF_DrawText(int x, int y, const char *str);
int SJF_TextExtents(const char *str);

#endif

