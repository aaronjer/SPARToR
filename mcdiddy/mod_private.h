/* PRIVATE STUFF the ENGINE shouldn't KNOW ABOUT! */


#include "SDL.h"
#include "main.h"


//obj types
enum { OBJT_MOTHER = 1,
       OBJT_GHOST,
       OBJT_DUMMY,
       OBJT_PLAYER,
       OBJT_BULLET,
       OBJT_SLUG,
       OBJT_AMIGO,
       OBJT_AMIGOSWORD };

//cmd types
enum { CMDT_1LEFT = 1,
       CMDT_0LEFT,
       CMDT_1RIGHT,
       CMDT_0RIGHT,
       CMDT_1UP,
       CMDT_0UP,
       CMDT_1DOWN,
       CMDT_0DOWN,
       CMDT_1JUMP,
       CMDT_0JUMP,
       CMDT_1FIRE,
       CMDT_0FIRE };


typedef struct{
  int camx;
  int camy;
} MOTHER_t;

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
  char stabbing;
  char hovertime;
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
  int dead;
} SLUG_t;

typedef struct{
  V pos;
  V vel;
  V hull[2];
  int model;
} DUMMY_t;

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

typedef struct{
  V pos;
  V vel;
  V hull[2];
  int model;
  int owner;
  int spincounter;
} AMIGOSWORD_t;

