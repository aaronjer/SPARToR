

#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "main.h"
#include "console.h"
#include "net.h"


void mod_adv(int a,int b,OBJ_t *oa,OBJ_t *ob) {
  int i;
  int slot0,slot1;
  GHOST_t *gh;
  PLAYER_t *pl;
  switch( ob->type ) {
  case OBJT_MOTHER:
    //the mother object is sort of the root of the object tree... only not... and there's no object tree
    for(i=0;i<maxclients;i++) {
      if( fr[b].cmds[i].flags & CMDF_NEW ) {
        SJC_Write("***NEW CLIENT DETECTED***");
        slot0 = findfreeslot(b);
        slot1 = findfreeslot(b);
        SJC_Write("%d,%d",slot0,slot1);
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
      }
    }
    break;
  case OBJT_GHOST:
    assert("ob->size==sizeof(GHOST_t)",ob->size==sizeof(GHOST_t));
    gh = ob->data;
    break;
  case OBJT_PLAYER:
    assert("ob->size==sizeof(PLAYER_t)",ob->size==sizeof(PLAYER_t));
    pl = ob->data;
    gh = fr[b].objs[pl->ghost].data;
    switch( fr[b].cmds[gh->client].cmd ) {
      case CMDT_1LEFT:  pl->goingl = 1; break;
      case CMDT_0LEFT:  pl->goingl = 0; break;
      case CMDT_1RIGHT: pl->goingr = 1; break;
      case CMDT_0RIGHT: pl->goingr = 0; break; 
      case CMDT_1UP:    pl->goingu = 1; break;
      case CMDT_0UP:    pl->goingu = 0; break;
      case CMDT_1DOWN:  pl->goingd = 1; break;
      case CMDT_0DOWN:  pl->goingd = 0; break; 
      case CMDT_1JUMP:  pl->jumping = 1; break;
      case CMDT_0JUMP:  pl->jumping = 0; break;
    }
    if( pl->goingl ) pl->pos.x -= 3.0f;
    if( pl->goingr ) pl->pos.x += 3.0f;
    if( pl->goingu ) pl->pos.y -= 3.0f;
    if( pl->goingd ) pl->pos.y += 3.0f;

    pl->vel.y += 0.5f;      //gravity
    pl->pos.x += pl->vel.x; //apply velocity
    pl->pos.y += pl->vel.y - pl->jumpvel;
    if( pl->jumpvel>0.0f )  //jumpvel fades away
      pl->jumpvel -= 3.0f;
    if( pl->jumpvel<0.0f ) {//end influence of jump, jumpvel can only be non-negative
      pl->jumpvel = 0.0f;
      pl->jumping = 0;      //must press jump again now
    }
    if( !pl->jumping )      //low-jump
      pl->jumpvel = 0.0f;
    if( pl->pos.y>400.0f ) {//on the ground
      pl->pos.y = 400.0f;
      pl->vel.y = 0;
      if( pl->jumping )     //initiate jump!
        pl->jumpvel = 10.0f;
    }
    break;
  }
}


