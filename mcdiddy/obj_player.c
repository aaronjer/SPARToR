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

//FIXME REMOVE! change local player model
int    setmodel = -1;
//

void obj_player_draw( int objid, OBJ_t *o )
{
  PLAYER_t *pl = o->data;
  int gunshift;
  if( pl->goingu ) gunshift = 96;
  if( pl->goingd ) gunshift = 48;
  if( pl->goingu==pl->goingd ) gunshift = 0;
  SDL_Rect drect = (SDL_Rect){(pl->pos.x-10),(pl->pos.y-15),0,0};
  int z = drect.y + pl->hull[1].y;
  int duck = pl->goingd>0 ? 40 : 0;

  if( pl->facingr ) {
    if( pl->model==4 ) //girl hair
      SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){ 80,120,20,15},
                                            &(SDL_Rect){drect.x-4,drect.y+(pl->goingd?4:0)+pl->gundown/7,0,0}, scale, z);
    SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){ 0+duck, 0+pl->model*30,20,30}, &drect, scale, z);
    drect = (SDL_Rect){(pl->pos.x- 5-pl->gunback),(pl->pos.y-10+pl->gundown/5),0,0};
    if( !pl->stabbing ) //gun
      SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){ 0+gunshift,150,24,27}, &drect, scale, z);
  } else {
    if( pl->model==4 ) //girl hair
      SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){100,120,20,15},
                                            &(SDL_Rect){drect.x+4,drect.y+(pl->goingd?4:0)+pl->gundown/7,0,0}, scale, z);
    SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){20+duck, 0+pl->model*30,20,30}, &drect, scale, z);
    drect = (SDL_Rect){(pl->pos.x-19+pl->gunback),(pl->pos.y-10+pl->gundown/5),0,0};
    if( !pl->stabbing ) //gun
      SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){24+gunshift,150,24,27}, &drect, scale, z);
  }

  if( pl->stabbing ) { //up/down stabbing
    drect = (SDL_Rect){pl->pos.x-2,pl->pos.y-(pl->stabbing<0?30:0),0,0};
    SJGL_BlitScaled(textures[TEX_PLAYER], &(SDL_Rect){148+(pl->stabbing<0?5:0),150,5,27}, &drect, scale, z);
  }
}

void obj_player_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  int i;
  int slot0;
  PLAYER_t *oldme = oa->data;
  PLAYER_t *newme = ob->data;
  GHOST_t *gh = fr[b].objs[newme->ghost].data;

  switch( fr[b].cmds[gh->client].cmd ) {
    case CMDT_1LEFT:  newme->goingl  = 1; newme->facingr = 0;  break;
    case CMDT_0LEFT:  newme->goingl  = 0;                      break;
    case CMDT_1RIGHT: newme->goingr  = 1; newme->facingr = 1;  break;
    case CMDT_0RIGHT: newme->goingr  = 0;                      break;
    case CMDT_1UP:    newme->goingu  = 1;                      break;
    case CMDT_0UP:    newme->goingu  = 0;                      break;
    case CMDT_1DOWN:  newme->goingd  = 1;                      break;
    case CMDT_0DOWN:  newme->goingd  = 0;                      break;
    case CMDT_1JUMP:  newme->jumping = 1;                      break;
    case CMDT_0JUMP:  newme->jumping = 0;                      break;
    case CMDT_1FIRE:  newme->firing  = 1;                      break;
    case CMDT_0FIRE:  newme->firing  = 0; newme->cooldown = 0; break;
  }

  if( !oldme ) { //FIXME why's this null?
    SJC_Write("Warning: oldme is NULL!");
    return;
  }

  gh->vel.x = newme->pos.x - gh->pos.x; //put ghost in the right spot
  gh->vel.y = newme->pos.y - gh->pos.y;

  if( ((GHOST_t *)fr[b].objs[newme->ghost].data)->client==me ) { //local client match
    v_camx = gh->pos.x;
    v_camy = gh->pos.y;
    if( setmodel>-1 ) { //FIXME -- just for fun, will not sync!
      newme->model = setmodel;
      setmodel = -1;
    }
  }

  if( newme->firing || (newme->stabbing<0 && !newme->goingu)
                    || (newme->stabbing>0 && !newme->goingd) ) //firing or stopping pressing up/down
    newme->stabbing = 0;
  else if( !newme->stabbing && newme->vel.y!=0.0 ) {       //freefalling, not stabbing
    if( newme->goingu && !oldme->goingu )                  //just pressed up
      newme->stabbing = -4;
    if( newme->goingd && !oldme->goingd )                  //just pressed down
      newme->stabbing = 4;
  }
  if( newme->stabbing==-1 || newme->stabbing==1 )          //last frame of stabbing is over
    newme->stabbing = 0;
  if( newme->stabbing && newme->vel.y==0.0 ) {             //tink tink tink!
    if( newme->stabbing>0 )
      newme->vel.y -= 0.8f*oldme->vel.y;
    newme->stabbing += (newme->stabbing>0 ? -1 : 1);
  }
  if(        newme->stabbing && newme->goingu ) {          //expand hull for stabbing
    newme->hull[0] = (V){-6.0f,-29.0f,0.0f};
    newme->hull[1] = (V){ 6.0f, 15.0f,0.0f};
  } else if( newme->stabbing && newme->goingd ) {
    newme->hull[0] = (V){-6.0f,-15.0f,0.0f};
    newme->hull[1] = (V){ 6.0f, 25.0f,0.0f};
  } else {
    newme->hull[0] = (V){-6.0f,-15.0f,0.0f};
    newme->hull[1] = (V){ 6.0f, 15.0f,0.0f};
  }

  newme->gunback = 0; //reset gun position
  if(newme->goingr||newme->goingl)
    newme->gundown = (newme->gundown+1)%10;
  else
    newme->gundown = 0;

  // friction
  if(      newme->vel.x> 0.2f ) newme->vel.x -= 0.2f;
  else if( newme->vel.x>-0.2f ) newme->vel.x  = 0.0f;
  else                          newme->vel.x += 0.2f;
  if(      newme->pvel.x> 0.5f ) newme->pvel.x -= 0.5f;
  else if( newme->pvel.x>-0.5f ) newme->pvel.x  = 0.0f;
  else                           newme->pvel.x += 0.5f;

  // -- WALK --
  if( newme->goingl ) {
    if(      newme->pvel.x>-2.0f ) newme->pvel.x += -1.0f;
    else if( newme->pvel.x>-3.0f ) newme->pvel.x  = -3.0f;
  }
  if( newme->goingr ) {
    if(      newme->pvel.x< 2.0f ) newme->pvel.x +=  1.0f;
    else if( newme->pvel.x< 3.0f ) newme->pvel.x  =  3.0f;
  }

  // -- JUMP --
  if( newme->pvel.y <= -2.0f ) {     //jumping in progress
    newme->pvel.y   +=  2.0f;        //jumpvel fades into real velocity
    newme->vel.y    += -2.0f;
  }
  else if( newme->pvel.y < 0.0f ) {  //jumping ending
    newme->vel.y    += newme->pvel.y;
    newme->pvel.y   = 0.0f;
    newme->jumping  = 0;             //must press jump again now
  }
  if( !newme->jumping )              //low-jump, cancel jump velocity early
    newme->pvel.y   = 0.0f;
  if( (newme->vel.y==0.0f || oldme->vel.y==0.0f) && newme->jumping ) //FIXME 0 velocity means grounded? not really
    newme->pvel.y  = -9.1f;         //initiate jump!

  // -- FIRE --
  if( newme->cooldown>0 )
    newme->cooldown--;
  if( newme->firing && newme->cooldown==0 && newme->projectiles<5 ) { // create bullet
    MKOBJ( bu, BULLET, ob->context, OBJF_POS|OBJF_VEL|OBJF_VIS );
    if( newme->facingr ) {
      bu->pos = (V){newme->pos.x+19.0f,newme->pos.y-3.0f,0.0f};
      bu->vel = (V){ 8.0f,0.0f,0.0f};
    } else {
      bu->pos = (V){newme->pos.x-19.0f,newme->pos.y-3.0f,0.0f};
      bu->vel = (V){-8.0f,0.0f,0.0f};
    }
    if( newme->goingu ) { // aiming
      bu->vel.y += -8.0f;
      bu->pos.x += -2.0f;
      bu->pos.y += -7.0f;
    }
    if( newme->goingd ) {
      bu->vel.y +=  8.0f;
      bu->pos.y += 16.0f;
    }
    bu->model = 1;
    bu->owner = objid;
    bu->ttl = 50;
    newme->cooldown = 5;
    newme->projectiles++;
    newme->gunback = 2;
  }

  for(i=0;i<objid;i++)  //find other players to interact with -- who've already MOVED
    if(fr[b].objs[i].type==OBJT_PLAYER) {
      PLAYER_t *oldyou = fr[a].objs[i].data;
      PLAYER_t *newyou = fr[b].objs[i].data;
      if(    !oldyou
          || fabsf(newme->pos.x - newyou->pos.x)>5.0f //we're not on top of each other
          || fabsf(newme->pos.y - newyou->pos.y)>2.0f
          || newme->goingr  ||  newme->goingl         //or we're moving
          || newyou->goingr || newyou->goingl         )
        continue;
      if(newme->pos.x < newyou->pos.x) {
        newme->pvel.x  -= 0.2f;
        newyou->pvel.x += 0.2f;
      } else {
        newme->pvel.x  += 0.2f;
        newyou->pvel.x -= 0.2f;
      }
    }

  if( newme->hovertime ) { //gravity?
    newme->hovertime--;
    newme->vel.y += 0.3f;
  } else
    newme->vel.y += newme->hovertime ? 0.3f : 0.7f;
}

