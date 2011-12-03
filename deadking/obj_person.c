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

void obj_person_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  PERSON_t *pe = o->data;
  int c = POINT2NATIVE_X(pe->pos)-10;
  int d = POINT2NATIVE_Y(pe->pos);
  SJGL_SetTex( sys_tex[TEX_PERSON].num );
  SJGL_Blit( &(SDL_Rect){20,0  ,20,34}, c, d-34, d   );

  // draw shadow
  SJGL_Blit( &(SDL_Rect){0 ,246,20,10}, c, d-5 , d-1 );
}

void obj_person_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  PERSON_t  *oldpe = oa->data;
  PERSON_t  *newpe = ob->data;
  CONTEXT_t *co = fr[b].objs[ob->context].data;
  int w  = 0,e  = 0,n  = 0,s  = 0;
  int nw = 0,ne = 0,sw = 0,se = 0;

  if( newpe->ghost )
  {
    GHOST_t *gh = fr[b].objs[newpe->ghost].data;

    switch( fr[b].cmds[gh->client].cmd ) {
      case CMDT_1LEFT:   w=1; break;
      case CMDT_1RIGHT:  e=1; break;
      case CMDT_1UP:     n=1; break;
      case CMDT_1DOWN:   s=1; break;
      case CMDT_1NW:    nw=1; break;
      case CMDT_1NE:    ne=1; break;
      case CMDT_1SW:    sw=1; break;
      case CMDT_1SE:    se=1; break;
      case CMDT_1SEL:         break;
      case CMDT_1BACK:        break;
    }
  }

  if( !oldpe ) { //FIXME why's this null?
    SJC_Write("Warning: oldpe is NULL!");
    return;
  }

  int newx, newz;
  if( w ) {
    newx = newpe->tilex-1;
    newz = newpe->tilez+1;
  } else if( e ) {
    newx = newpe->tilex+1;
    newz = newpe->tilez-1;
  } else if( n ) {
    newx = newpe->tilex-1;
    newz = newpe->tilez-1;
  } else if( s ) {
    newx = newpe->tilex+1;
    newz = newpe->tilez+1;
  }

  if( newx>=0 && newz>=0 && newx<co->x && newz<co->z ) {
    newpe->tilex = newx;
    newpe->tilez = newz;
  }

  float posx = newpe->tilex*co->bsx + co->bsx/2;
  float posz = newpe->tilez*co->bsz + co->bsz/2;

  float velx = (posx - newpe->pos.x)/4;
  float velz = (posz - newpe->pos.z)/4;
  float mag = sqrtf(velx*velx + velz*velz);

  // just snap if close
  if( fabsf(velx)<0.5 && fabsf(velz)<0.5 ) {
    newpe->vel.x = 0;
    newpe->vel.z = 0;
    newpe->pos.x = posx;
    newpe->pos.z = posz;
  } else if( mag<2 ) {
    newpe->vel.x = velx/mag*2;
    newpe->vel.z = velz/mag*2;
  } else {
    newpe->vel.x = velx;
    newpe->vel.z = velz;
  }

  newpe->vel.y += 0.6f;      //gravity
}

