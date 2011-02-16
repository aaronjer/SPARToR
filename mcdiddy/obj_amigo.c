/**
 **  mcdiddy's game
 **  implementation example for the spartor network game engine
 **  copyright (c) 2010-2011  jer wilson
 **
 **  see copying for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/spartor
 **/

#include "obj_.h"

//FIXME REMOVE! force amigo to flykick
int flykick = 0;
//

void obj_amigo_draw( int objid, OBJ_t *o )
{
  typedef struct { int x, y, w, h, dx, dy; } XSPR;
  XSPR tip = {0}; // extra sprite for tip of sword
  AMIGO_t *am = o->data;
  int w = 50;
  int h = 50;
  int x = 0,y = 0;
  int z = am->pos.y + am->hull[1].y;
  switch( am->state ) {
    case AMIGO_HELLO:
      x = (am->statetime/30) * 50; //                       x   y  w  h  dx  dy
      if(      am->statetime<30 ) { x =   0; tip = (XSPR){0}; }
      else if( am->statetime<60 ) { x =  50; tip = (XSPR){0}; }
      else if( am->statetime<90 ) { x = 100; tip = (XSPR){0}; }
      else                        { x = 150; tip = (XSPR){220,230,10,10,  8,-10}; }
      y = 150;
      break;
    case AMIGO_COOLDOWN:
      tip = (XSPR){210,230,10,10, 20,-10};
      break;
    case AMIGO_JUMP:
      tip = (XSPR){240,230,10,10, 33,-10};
      x = 0;
      y = 50;
      break;
    case AMIGO_SLASH: //                                    x   y  w  h  dx  dy
      if(      am->statetime<20 ) { x =  50; tip = (XSPR){220,240,30,10,-30, 42}; }
      else if( am->statetime<25 ) { x = 100; tip = (XSPR){220,230,10,10,  6,-10}; }
      else                        { x = 150; tip = (XSPR){230,230,10,10, 32,-10}; }
      break;
    case AMIGO_FLYKICK:
      if( am->statetime > 30 && am->sword_dist.x < 80.0f && am->sword_dist.y < 60.0f )
        // in this state tip is amigo's left arm
        //             x   y  w  h  dx  dy
        tip = (XSPR){170,220,20,20, 35, 20};
      else
        tip = (XSPR){190,220,20,20, 25, 24};
      x = ((am->hatcounter%100)/50 ? 100 : 50);
      y = 50;
      z += 32;
      break;
    case AMIGO_DASH:
      tip = (XSPR){210,250,40, 6,-40, 40};
      x = 150;
      y = 50;
      break;
  }
  SJGL_BlitScaled(textures[TEX_AMIGO], &(SDL_Rect){     x,     y,     w,     h },
                                        &(SDL_Rect){ am->pos.x-34,        am->pos.y-32,        0, 0 }, scale, z);
  SJGL_BlitScaled(textures[TEX_AMIGO], &(SDL_Rect){ tip.x, tip.y, tip.w, tip.h },
                                        &(SDL_Rect){ am->pos.x-34+tip.dx, am->pos.y-32+tip.dy, 0, 0 }, scale, z);
}

