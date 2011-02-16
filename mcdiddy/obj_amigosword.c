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

void obj_amigosword_draw( int objid, OBJ_t *o )
{
  AMIGOSWORD_t *sw = o->data;
  SJGL_BlitScaled(textures[TEX_AMIGO], &(SDL_Rect){ 200, 50+50*(hotfr%3), 56, 50 },
                                       &(SDL_Rect){ sw->pos.x-25, sw->pos.y-28, 0, 0 }, scale, sw->pos.y );
}

void obj_amigosword_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  AMIGOSWORD_t *sw = ob->data;
  sw->spincounter++;
  if( sw->spincounter > 45 ) {
    AMIGO_t *am = fr[b].objs[sw->owner].data;
    sw->vel.x = am->pos.x+40.0f - sw->pos.x;
    sw->vel.y = am->pos.y       - sw->pos.y;
    float normalize = sqrt(sw->vel.x * sw->vel.x + sw->vel.y * sw->vel.y);
    if( normalize > 4.0f ) {
      normalize = 4.0f / normalize;
      sw->vel.x *= normalize;
      sw->vel.y *= normalize;
    }
  }
}

