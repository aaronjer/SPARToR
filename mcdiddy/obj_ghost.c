/**
 **  McDiddy's Game
 **  Implementation example for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2011  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "obj_.h"

void obj_ghost_draw( int objid, OBJ_t *o )
{
  GHOST_t *gh = o->data;
  SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){80,177,16,16},
                                        &(SDL_Rect){gh->pos.x, gh->pos.y, 16, 16}, scale,NATIVEH);
}

void obj_ghost_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  //nothing to do... yet?
}

