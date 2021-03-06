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
#include "font.h"

SJF_t SJF = {0,8,12,128,
  { 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //non-printable
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   //non-printable
    6, 3, 5, 7, 7, 6, 7, 3, 5, 5, 6, 7, 3, 6, 3, 6,   //<space> - </>
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 5, 6, 5, 7,   //<0> - <?>
    7, 6, 6, 6, 6, 6, 6, 6, 6, 5, 6, 6, 6, 7, 7, 6,   //<@> - <O>
    6, 6, 6, 6, 7, 6, 7, 7, 7, 7, 6, 5, 6, 5, 7, 7,   //<P> - <_>
    4, 6, 6, 6, 6, 6, 5, 6, 6, 4, 5, 6, 4, 7, 6, 6,   //<`> - <o>
    6, 6, 5, 6, 5, 6, 7, 7, 7, 6, 6, 5, 3, 5, 8, 0 }, //<p> - <127>
{
"                                                                                                                                "
"                                                                                                                                "
" OOOO            OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO   "
" O  O            O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    OOO     O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    OOO     O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    OOO     O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O            O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" OOOO            OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO   "
"                                                                                                                                "
"                                                                                                                                "
"                                                                                                                                "

"                                                                                                                                "
"                                                                                                                                "
" OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO   "
" O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O    O  O   "
" OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO    OOOO   "
"                                                                                                                                "
"                                                                                                                                "
"                                                                                                                                "

"                                                                                                                                "
"                                                                                                                                "
"         O       O O               O                     O         O     O                                                  O   "
"         O       O O      O O     OOO             OO     O        O       O      O  O      O                                O   "
"         O               OOOOO   O O             O  O            O         O      OO       O                               O    "
"         O                O O     OOO    O  O     OO             O         O     OOOO    OOOOO           OOOO              O    "
"         O                O O      O O     O     O  OO           O         O      OO       O                              O     "
"                         OOOOO    OOO     O      O  O             O       O      O  O      O                              O     "
"         O                O O      O     O  O     OO O             O     O                       O               O       O      "
"                                                                                                 O                       O      "
"                                                                                                                                "
"                                                                                                                                "

"                                                                                                                                "
"                                                                                                                                "
"                                                                                                                          OOO   "
"  OO       O      OO      OO     O       OOOO     OO     OOOO     OO      OO                       O             O       O   O  "
" O  O     OO     O  O    O  O    O  O    O       O          O    O  O    O  O                     O      OOOO     O          O  "
" O OO      O        O      O     O  O    OOO     OOO       O      OO     O  O    O       O       O                 O        O   "
" OO O      O      OO        O    OOOO       O    O  O      O     O  O     OOO                     O      OOOO     O        O    "
" O  O      O     O       O  O       O    O  O    O  O     O      O  O       O                      O             O              "
"  OO      OOO    OOOO     OO        O     OO      OO      O       OO      OO     O       O                                 O    "
"                                                                                         O                                      "
"                                                                                                                                "
"                                                                                                                                "

"                                                                                                                                "
"                                                                                                                                "
"  OOO     OO     OOO      OO     OOO     OOOO    OOOO     OO     O  O    OOO       OO    O  O    O       O   O   O   O    OO    "
" O   O   O  O    O  O    O  O    O  O    O       O       O  O    O  O     O         O    O  O    O       OO OO   OO  O   O  O   "
" O OOO   O  O    O  O    O       O  O    O       O       O       O  O     O         O    O O     O       O O O   O O O   O  O   "
" O O O   OOOO    OOO     O       O  O    OOO     OOO     O OO    OOOO     O         O    OO      O       O O O   O  OO   O  O   "
" O OOO   O  O    O  O    O       O  O    O       O       O  O    O  O     O         O    O O     O       O   O   O   O   O  O   "
" O       O  O    O  O    O  O    O  O    O       O       O  O    O  O     O      O  O    O  O    O       O   O   O   O   O  O   "
"  OOO    O  O    OOO      OO     OOO     OOOO    O        OOO    O  O    OOO      OO     O  O    OOOO    O   O   O   O    OO    "
"                                                                                                                                "
"                                                                                                                                "
"                                                                                                                                "

"                                                                                                                                "
"                                                                                                                                "
" OOO      OO     OOO      OOO    OOOOO   O  O    O   O   O   O   O   O   O   O   OOOO    OOO     O       OOO       O            "
" O  O    O  O    O  O    O         O     O  O    O   O   O   O   O   O   O   O      O    O       O         O      O O           "
" O  O    O  O    O  O    O         O     O  O    O   O   O   O    O O    O   O     O     O        O        O     O   O          "
" OOO     O  O    OOO      OO       O     O  O    O   O   O O O     O      OOO     O      O        O        O                    "
" O       O  O    O O        O      O     O  O     O O    O O O    O O      O     O       O         O       O                    "
" O       O  O    O  O       O      O     O  O     O O    O O O   O   O     O     O       O         O       O                    "
" O        OO     O  O    OOO       O      OO       O      O O    O   O     O     OOOO    OOO        O    OOO                    "
"            O                                                                                       O                    OOOOO  "
"                                                                                                                                "
"                                                                                                                                "

"                                                                                                                                "
"                                                                                                                                "
" O               O                  O              O             O        O        O     O       OO                             "
"  O              O                  O             O              O                       O        O                             "
"          OO     OOO      OOO     OOO     OO     OOO      OOO    OOO     OO       OO     O  O     O      OOOO    OOO      OO    "
"         O  O    O  O    O       O  O    O  O     O      O  O    O  O     O        O     O O      O      O O O   O  O    O  O   "
"          OOO    O  O    O       O  O    OOOO     O      O  O    O  O     O        O     OO       O      O O O   O  O    O  O   "
"         O  O    O  O    O       O  O    O        O      O  O    O  O     O        O     O O      O      O O O   O  O    O  O   "
"          OOO    OOO      OOO     OOO     OOO     O       OOO    O  O     O        O     O  O     O      O O O   O  O     OO    "
"                                                            O                      O                                            "
"                                                          OO                     OO                                             "
"                                                                                                                                "

"                                                                                                                                "
"                                                                                                                                "
"                                                                                           O     O       O        OO  O  OOOO   "
"                                  O                                                       O      O        O      O  OO   O  O   "
" OOO      OOO    O O      OOO    OOO     O  O    O   O   O O O   O   O   O  O    OOOO     O      O        O              O  O   "
" O  O    O  O    OO      O        O      O  O    O   O   O O O    O O    O  O       O    O       O         O             O  O   "
" O  O    O  O    O        OO      O      O  O     O O    O O O     O     O  O     OO      O               O              O  O   "
" O  O    O  O    O          O     O      O  O     O O    O O O    O O    O  O    O        O      O        O              O  O   "
" OOO      OOO    O       OOO      OO      OOO      O      OOOO   O   O    OOO    OOOO      O     O       O               OOOO   "
" O          O                                                               O                    O                              "
" O          O                                                             OO                     O                              "
"                                                                                                                                "

"                                                                                                                                "
"                                                                                                                                "
  }};


//initializes the SuperJer Font library
void SJF_Init()
{
  Uint32 pixels[256*256];
  Uint32 u;
  Uint32 v;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  Uint32 black = 0x000000FF, white = 0xFFFFFFFF, clear = 0x00000000;
#else
  Uint32 black = 0xFF000000, white = 0xFFFFFFFF, clear = 0x00000000;
#endif

  for(u=0; u<128; u++)
    for(v=0; v<128; v++)
      if( SJF.raw[u+v*128]!=' ' )
        pixels[u+v*256] = white;
      else if( (u<127 && SJF.raw[(u+1)+(v  )*128]!=' ')
            || (u>0   && SJF.raw[(u-1)+(v  )*128]!=' ')
            || (v<127 && SJF.raw[(u  )+(v+1)*128]!=' ')
            || (v>0   && SJF.raw[(u  )+(v-1)*128]!=' ') )
        pixels[u+v*256] = black;
      else
        pixels[u+v*256] = clear;

  //make into a GL texture
  glPixelStorei(GL_UNPACK_ALIGNMENT,4);
  glDeleteTextures(1,&SJF.tex);
  glGenTextures(1,&SJF.tex);

  glBindTexture(GL_TEXTURE_2D,SJF.tex);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}


//draws a single character in system text in GL
void SJF_DrawChar(int x, int y, char ch)
{
  SDL_Rect s = (SDL_Rect){ (ch%16)*SJF.w, (ch/16)*SJF.h, 8, 12 };
  SDL_Rect d = (SDL_Rect){             x,             y, 8, 12 };

  glBindTexture(GL_TEXTURE_2D,0); //FIXME: hack 4 win
  glBindTexture(GL_TEXTURE_2D,SJF.tex);
  glBegin(GL_QUADS);
  glTexCoord2i(s.x    ,s.y    ); glVertex2i(d.x    ,d.y    );
  glTexCoord2i(s.x+s.w,s.y    ); glVertex2i(d.x+d.w,d.y    );
  glTexCoord2i(s.x+s.w,s.y+s.h); glVertex2i(d.x+d.w,d.y+d.h);
  glTexCoord2i(s.x    ,s.y+s.h); glVertex2i(d.x    ,d.y+d.h);
  glEnd();
}


//draws a message in system text at location in GL
void SJF_DrawText(int x, int y, const char *str)
{
  SDL_Rect s = (SDL_Rect){ 0, 0, 0, 12 };
  SDL_Rect d = (SDL_Rect){ x, y, 0, 12 };

  if( !str ) str = "(null)";

  glBindTexture(GL_TEXTURE_2D,0); //FIXME: hack 4 win
  glBindTexture(GL_TEXTURE_2D,SJF.tex);
  glBegin(GL_QUADS);
  while( *str )
  {
    s.x = (*str%16)*SJF.w;
    s.y = (*str/16)*SJF.h;
    s.w = SJF.space[(Uint8)*str];
    d.w = s.w;
    glTexCoord2i(s.x    ,s.y    ); glVertex2i(d.x    ,d.y    );
    glTexCoord2i(s.x+s.w,s.y    ); glVertex2i(d.x+d.w,d.y    );
    glTexCoord2i(s.x+s.w,s.y+s.h); glVertex2i(d.x+d.w,d.y+d.h);
    glTexCoord2i(s.x    ,s.y+s.h); glVertex2i(d.x    ,d.y+d.h);
    d.x += s.w-1;
    str++;
  }
  glEnd();
}


//returns number of pixels text will consume horizontally
//non-printable characters will cause unexpected behavior
int SJF_TextExtents(const char *str)
{
  int n = 0;
  if( str==NULL )
    return 0;
  while(*str)
    n += SJF.space[(Uint8)*str++]-1;
  return n;
}


