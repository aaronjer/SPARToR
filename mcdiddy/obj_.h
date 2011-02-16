//
// This is the header for all obj_*.c files
//


#ifndef MCDIDDY_OBJ_H_
#define MCDIDDY_OBJ_H_


#include "mod.h"


//obj types
enum { OBJT_MOTHER = 1,
       OBJT_GHOST,
       OBJT_DUMMY,
       OBJT_PLAYER,
       OBJT_BULLET,
       OBJT_SLUG,
       OBJT_AMIGO,
       OBJT_AMIGOSWORD };


// AMIGO //
enum { AMIGO_HELLO,
       AMIGO_COOLDOWN,
       AMIGO_JUMP,
       AMIGO_SLASH,
       AMIGO_FLYKICK,
       AMIGO_DASH };

typedef struct{
  V pos;
  V vel;
  V hull[2];
  int model;
  char state;
  int statetime;
  int hatcounter;
  int sword;
  V sword_dist;
} AMIGO_t;

void obj_amigo_draw( int objid, OBJ_t *o );


#endif

