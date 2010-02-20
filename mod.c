

#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
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
        fr[b].objs[slot1].flags = OBJF_POS|OBJF_VIS;
        fr[b].objs[slot1].size = sizeof(PLAYER_t);
        pl = fr[b].objs[slot1].data = malloc(sizeof(PLAYER_t));
        pl->pos = (V){200.0f,200.0f,0.0f};
        pl->vel = (V){0.0f,0.0f,0.0f};
        pl->jumpvel = 0.0f;
        pl->model = 1;
        pl->ghost = slot0;
        pl->goingl = 0;
        pl->goingr = 0;
        pl->goingu = 0;
        pl->goingd = 0;
        pl->jumping = 0;
        pl->grounded = 0;
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
      case CMDT_1LEFT:  newme->goingl = 1; break;
      case CMDT_0LEFT:  newme->goingl = 0; break;
      case CMDT_1RIGHT: newme->goingr = 1; break;
      case CMDT_0RIGHT: newme->goingr = 0; break; 
      case CMDT_1UP:    newme->goingu = 1; break;
      case CMDT_0UP:    newme->goingu = 0; break;
      case CMDT_1DOWN:  newme->goingd = 1; break;
      case CMDT_0DOWN:  newme->goingd = 0; break; 
      case CMDT_1JUMP:  newme->jumping = 1; break;
      case CMDT_0JUMP:  newme->jumping = 0; break;
    }
    if( newme->goingl ) newme->pos.x -= 3.0f;
    if( newme->goingr ) newme->pos.x += 3.0f;
    if( newme->goingu ) newme->pos.y -= 3.0f;
    if( newme->goingd ) newme->pos.y += 3.0f;

    newme->grounded = 0;          //in the air until proven otherwise

    if( newme->jumpvel>0.0f )     //jumpvel fades away
      newme->jumpvel -= 3.0f;
    if( newme->jumpvel<0.0f ) {   //end influence of jump, jumpvel can only be non-negative
      newme->jumpvel = 0.0f;
      newme->jumping = 0;         //must press jump again now
    }
    if( !newme->jumping )         //low-jump
      newme->jumpvel = 0.0f;

    newme->vel.y += 0.8f;         //gravity and jump effect
    newme->vel.y -= newme->jumpvel;

    newme->pos.x += newme->vel.x; //apply velocity
    newme->pos.y += newme->vel.y;

    if( newme->pos.x<0.0f )       //screen edges
      newme->pos.x = 0.0f;
    if( newme->pos.x>640.0f )
      newme->pos.x = 640.0f;

    float floor = 400.0f;         //ground test
    if( newme->pos.y>floor ) {    //on the ground
      newme->pos.y = floor;
      newme->grounded = 1;
      newme->vel.y = 0.0f;
    }

    if( !oldme ) //FIXME why's this null?
      break;

    if( newme->grounded || oldme->grounded )
    {
      if( newme->jumping )        //initiate jump!
        newme->jumpvel = 8.0f;
    }

    for(i=0;i<objid;i++)  //find other players to interact with -- who've already MOVED
      if(fr[b].objs[i].type==OBJT_PLAYER) {
        PLAYER_t *oldyou = fr[a].objs[i].data;
        PLAYER_t *newyou = fr[b].objs[i].data;
        if( fabsf(newme->pos.x - newyou->pos.x)<=5.0f &&                   //we're on top of each other
            fabsf(newme->pos.y - newyou->pos.y)<=2.0f &&                   //pretty much
            newme->pos.x==oldme->pos.x && newyou->pos.x==oldyou->pos.x ) { //and not moving
          if(newme->pos.x < newyou->pos.x ) {
            newme->pos.x -= 1.0f;
            newyou->pos.x += 1.0f;
          } else {
            newme->pos.x += 1.0f;
            newyou->pos.x -= 1.0f;
          } 
        }
      }
    break;
  }
}


