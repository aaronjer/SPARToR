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
#include "font.h"

SJF_t SJF = {NULL,8,12,128,
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
  const SDL_VideoInfo *vidinfo;
  Uint32 u;
  Uint32 v;

  vidinfo = SDL_GetVideoInfo();

  SJF.surf = SDL_CreateRGBSurface(SDL_SRCCOLORKEY,128,128,
                                  vidinfo->vfmt->BitsPerPixel,
                                  vidinfo->vfmt->Rmask,
                                  vidinfo->vfmt->Gmask,
                                  vidinfo->vfmt->Bmask,
                                  vidinfo->vfmt->Amask);
  SDL_SetColorKey(SJF.surf,SDL_SRCCOLORKEY,SDL_MapRGB(SJF.surf->format,0,0,0));
  SDL_FillRect(SJF.surf,NULL,SDL_MapRGB(SJF.surf->format,0,0,0));
  SDL_LockSurface(SJF.surf);
  for(u=0; u<128; u++)
    for(v=0; v<128; v++)
      if( SJF.raw[u+v*128]!=' ' )
        SJDL_SetPixel(SJF.surf, u, v, 255, 255, 255);
      else if( (u<127 && SJF.raw[(u+1)+(v  )*128]!=' ')
            || (u>0   && SJF.raw[(u-1)+(v  )*128]!=' ')
            || (v<127 && SJF.raw[(u  )+(v+1)*128]!=' ')
            || (v>0   && SJF.raw[(u  )+(v-1)*128]!=' ') )
        SJDL_SetPixel( SJF.surf, u, v, 0, 0, 1 ); 
  SDL_UnlockSurface(SJF.surf);
}


//draws a single character in system text on a surface 
void SJF_DrawChar(SDL_Surface *surf, int x, int y, char c)
{
  SDL_Rect src;
  SDL_Rect dst;

  src.x = (c%16)*SJF.w;
  src.y = ((c)/16)*SJF.h;
  src.w = 8;
  src.h = 12;

  dst.x = x;
  dst.y = y;
  dst.w = 8;
  dst.h = 12;

  SDL_BlitSurface(SJF.surf,&src,surf,&dst);
}


//draws a message in system text at location on a surface
void SJF_DrawText(SDL_Surface *surf, int x, int y, const char *s)
{
  SDL_Rect src;
  SDL_Rect dst;

  src.h = 12;
  dst.x = x;
  dst.y = y;
  dst.h = 12;

  while( *s )
  {
    src.x = (*s%16)*SJF.w;
    src.y = ((*s)/16)*SJF.h;
        src.w = SJF.space[(Uint8)*s];
        dst.w = src.w;
    SDL_UpperBlit(SJF.surf,&src,surf,&dst);
    dst.x += src.w-1;
    s++;
  }
}


//returns number of pixels text will consume horizontally
//non-printable characters will cause unexpected behavior
int SJF_TextExtents(const char *s)
{
  int n = 0;
  if( s==NULL )
    return 0;
  while(*s)
    n += SJF.space[(Uint8)*s++]-1;
  return n;
}


