/**
 **  SPARToR 
 **  Network Game Engine
 **  Copyright (C) 2010  Jer Wilson
 **
 **  See LICENSE for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/


#include "SDL.h"
#include "main.h"
#include "console.h"
#include "font.h"
#include "video.h"


void run_tests()
{
  DrawSquare(screen,&(SDL_Rect){20,210,120,120},0xFF0000);
  creatables += 3;
}


