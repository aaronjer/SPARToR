/**
 **  Dead Kings' Quest
 **  A special game for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2011  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "obj_.h"

void obj_dummy_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  DUMMY_t *du = o->data;
  int x = TILE2NATIVE_X(co,(du->pos.x+du->hull[0].x)/16,0,(du->pos.y+du->hull[0].y)/16);
  int y = TILE2NATIVE_Y(co,(du->pos.x+du->hull[0].x)/16,0,(du->pos.y+du->hull[0].y)/16);
  int w = du->hull[1].x-du->hull[0].x;
  int h = du->hull[1].y-du->hull[0].y;

  Sint16 offs = w==h ? 48 : 0;

  SJGL_SetTex( sys_tex[TEX_WORLD].num );

  if( w > h ) while( w>0 && w<400 ) {
    SJGL_Blit( &(SDL_Rect){0+offs,16,16,16}, x, y, 0 );
    x += 16;
    w -= 16;
    offs = w==16 ? 32 : 16;
  } else                  while( h>0 && h<400 ) {
    SJGL_Blit( &(SDL_Rect){48,0+offs,16,16}, x, y, 0 );
    y += 16;
    h -= 16;
    offs = h==16 ? 32 : 16;
  }
}

void obj_dummy_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  DUMMY_t *du = ob->data;

  // friction
  if(      du->vel.x> 0.1f ) du->vel.x -= 0.1f;
  else if( du->vel.x>-0.1f ) du->vel.x  = 0.0f;
  else                       du->vel.x += 0.1f;

  if( objid==(int)(hotfr+100)%2000 ) //tee-hee
    du->vel.x += (float)(b%4)-1.5;

  du->vel.y += 0.7f;        //gravity
}

