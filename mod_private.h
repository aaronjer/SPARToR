/* PRIVATE STUFF the ENGINE shouldn't KNOW ABOUT! */


#include "SDL.h"
#include "main.h"


//obj types
#define OBJT_MOTHER   1
#define OBJT_GHOST    2
#define OBJT_DUMMY    3
#define OBJT_PLAYER   4
#define OBJT_BULLET   5

//cmd types
#define CMDT_1LEFT    ((char)1)
#define CMDT_0LEFT    ((char)2)
#define CMDT_1RIGHT   ((char)3)
#define CMDT_0RIGHT   ((char)4)
#define CMDT_1UP      ((char)5)
#define CMDT_0UP      ((char)6)
#define CMDT_1DOWN    ((char)7)
#define CMDT_0DOWN    ((char)8)
#define CMDT_1JUMP    ((char)9)
#define CMDT_0JUMP    ((char)10)
#define CMDT_1FIRE    ((char)11)
#define CMDT_0FIRE    ((char)12)



typedef struct{
  V pos;
  int client;
  int avatar;
} GHOST_t;

typedef struct{
  V pos;
  V vel;
  V hull[2];
  V pvel;
  int model;
  int ghost;
  char goingl;
  char goingr;
  char goingu;
  char goingd;
  char jumping;
  char firing;
  char cooldown;
  char projectiles;
  char gunback,gundown;
  char grounded;
  char facingr;
} PLAYER_t;

typedef struct{
  V pos;
  V vel;
  int model;
  int owner;
  Uint8 ttl;
} BULLET_t;

typedef struct{
  V pos;
  V vel;
  V hull[2];
  int model;
} DUMMY_t;

