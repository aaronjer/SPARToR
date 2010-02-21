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
#include "SDL_net.h"
#include "main.h"
#include "console.h"
#include "net.h"
#include <math.h>


void mod_adv(Uint32 objid,Uint32 a,Uint32 b,OBJ_t *oa,OBJ_t *ob) {
  int i;
  int slot0,slot1;
  GHOST_t *gh;
  PLAYER_t *pl;
  switch( ob->type ) {
  case OBJT_MOTHER:
    //the mother object is sort of the root of the object tree... only not... and there's no object tree
    for(i=0;i<maxclients;i++) {
      if( fr[b].cmds[i].flags & CMDF_NEW ) {
        int j;
        for(j=0;j<maxobjs;j++)
          if( fr[b].objs[j].type==OBJT_GHOST ) {
            gh = fr[b].objs[j].data;
            if(gh->client==i)
              SJC_Write("%d Mother(%d): Client %i already has a ghost at obj#%d!",hotfr,objid,i,j);
          }
        slot0 = findfreeslot(b);
        slot1 = findfreeslot(b);
        SJC_Write("%d Mother(%d): New client %d detected, created ghost is obj#%d, player is obj#%d",
                  hotfr,objid,i,slot0,slot1);
        fr[b].objs[slot0].type = OBJT_GHOST;
        fr[b].objs[slot0].flags = OBJF_POS;
        fr[b].objs[slot0].size = sizeof(GHOST_t);
        gh = fr[b].objs[slot0].data = malloc(sizeof(GHOST_t));
        gh->pos = (V){0.0f,0.0f,0.0f};
        gh->client = i;
        gh->avatar = slot1;
        fr[b].objs[slot1].type = OBJT_PLAYER;
        fr[b].objs[slot1].flags = OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_PVEL|OBJF_VIS;
        fr[b].objs[slot1].size = sizeof(PLAYER_t);
        pl = fr[b].objs[slot1].data = malloc(sizeof(PLAYER_t));
        pl->pos  = (V){200.0f,200.0f,0.0f};
        pl->vel  = (V){0.0f,0.0f,0.0f};
        pl->hull[0] = (V){-10.0f,-10.0f,0.0f};
        pl->hull[1] = (V){ 10.0f, 10.0f,0.0f};
        pl->pvel = (V){0.0f,0.0f,0.0f};
        pl->model = 1;
        pl->ghost = slot0;
        pl->goingl = 0;
        pl->goingr = 0;
        pl->goingu = 0;
        pl->goingd = 0;
        pl->jumping = 0;
        pl->firing = 0;
        pl->cooldown = 0;
        pl->grounded = 0;
        pl->facingr = 1;
      }
    }
    break;
  case OBJT_GHOST:
    assert("ob->size==sizeof(GHOST_t)",ob->size==sizeof(GHOST_t));
    gh = ob->data;
    break;
  case OBJT_DUMMY:
    ((V*)flex(ob,OBJF_POS))->x += (float)((hotfr-objid)%50) - 24.5f;
    break;
  case OBJT_PLAYER:
    assert("ob->size==sizeof(PLAYER_t)",ob->size==sizeof(PLAYER_t));
    PLAYER_t *oldme = oa->data;
    PLAYER_t *newme = ob->data;
    gh = fr[b].objs[newme->ghost].data;
    switch( fr[b].cmds[gh->client].cmd ) {
      case CMDT_1LEFT:  newme->goingl  = 1; newme->facingr = 0; break;
      case CMDT_0LEFT:  newme->goingl  = 0;                     break;
      case CMDT_1RIGHT: newme->goingr  = 1; newme->facingr = 1; break;
      case CMDT_0RIGHT: newme->goingr  = 0;                     break; 
      case CMDT_1UP:    newme->goingu  = 1;                     break;
      case CMDT_0UP:    newme->goingu  = 0;                     break;
      case CMDT_1DOWN:  newme->goingd  = 1;                     break;
      case CMDT_0DOWN:  newme->goingd  = 0;                     break; 
      case CMDT_1JUMP:  newme->jumping = 1;                     break;
      case CMDT_0JUMP:  newme->jumping = 0;                     break;
      case CMDT_1FIRE:  newme->firing  = 1;                     break;
      case CMDT_0FIRE:  newme->firing  = 0;                     break;
    }

    if( !oldme ) //FIXME why's this null?
      break;

    // friction
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
      newme->pvel.y  = -11.9f;         //initiate jump!

    // -- FIRE --
    if( newme->cooldown>0 )
      newme->cooldown--;
    if( newme->firing && newme->cooldown==0 && newme->projectiles<3 ) { // create bullet
      slot0 = findfreeslot(b);
      fr[b].objs[slot0].type = OBJT_BULLET;
      fr[b].objs[slot0].flags = OBJF_POS|OBJF_VEL|OBJF_HULL|OBJF_VIS;
      fr[b].objs[slot0].size = sizeof(BULLET_t);
      BULLET_t *bu = fr[b].objs[slot0].data = malloc(sizeof(BULLET_t));
      if( newme->facingr ) {
        bu->pos = (V){newme->pos.x+10.0f,newme->pos.y,0.0f};
        bu->vel = (V){ 8.0f,0.0f,0.0f};
      } else {
        bu->pos = (V){newme->pos.x-10.0f,newme->pos.y,0.0f};
        bu->vel = (V){-8.0f,0.0f,0.0f};
      }
      if( newme->goingu ) // aiming
        bu->vel.y +=-8.0f;
      if( newme->goingd )
        bu->vel.y += 8.0f;
      bu->hull[0] = (V){-2.0f,-2.0f,0.0f};
      bu->hull[1] = (V){ 2.0f, 2.0f,0.0f};
      bu->model = 1;
      bu->owner = objid;
      bu->ttl = 50;
      newme->cooldown = 5;
      newme->projectiles++;
    }

    for(i=0;i<objid;i++)  //find other players to interact with -- who've already MOVED
      if(fr[b].objs[i].type==OBJT_PLAYER) {
        PLAYER_t *oldyou = fr[a].objs[i].data;
        PLAYER_t *newyou = fr[b].objs[i].data;
        if( !oldyou                                  ||
            fabsf(newme->pos.x - newyou->pos.x)>5.0f || //we're not on top of each other
            fabsf(newme->pos.y - newyou->pos.y)>2.0f || 
            fabsf( newme->vel.x)>=1.0f               || //or we're moving
            fabsf(newyou->vel.x)>=1.0f )
          continue;
        if(newme->pos.x < newyou->pos.x) {
          newme->pvel.x  -= 1.0f;
          newyou->pvel.x += 1.0f;
        } else {
          newme->pvel.x  += 1.0f;
          newyou->pvel.x -= 1.0f;
        }
      }

    newme->vel.y += 0.8f;        //gravity
    break;
  case OBJT_BULLET:
    assert("ob->size==sizeof(BULLET_t)",ob->size==sizeof(BULLET_t));
    BULLET_t *bu = ob->data;
    bu->ttl--;
    if(bu->pos.x<=0.0f || bu->pos.x>=640.0f || bu->ttl==0) {
      if( fr[b].objs[bu->owner].type==OBJT_PLAYER )
        ((PLAYER_t *)fr[b].objs[bu->owner].data)->projectiles--;
      ob->flags |= OBJF_DEL;
    }
    break;
  } //end switch ob->type
}

