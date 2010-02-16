
#ifndef __MAIN_H__
#define __MAIN_H__

#include "SDL/SDL.h"
#include "SDL/SDL_net.h"

#define VERSION "0.1a"

#define TICKSAFRAME 30
#define PACKET_SIZE 5000

//macros
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define SWAP(a,b) {a ^= b;b ^= a;a ^= b;}
#define TRY do{
#define HARDER }while(0);

//obj flags
#define OBJF_POS 0x00000001
#define OBJF_VIS 0x00000002

//cmd flags
#define CMDF_LIV 0x00000001
#define CMDF_NEW 0x00000002

//obj types
#define OBJT_MOTHER   1
#define OBJT_GHOST    2
#define OBJT_DUMMY    3
#define OBJT_PLAYER   4

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


typedef struct{
  char cmd;
  char mousehi;
  char mousex;
  char mousey;
  short flags;
} FCMD_t;

typedef struct{
  short type;
  short flags;
  size_t size;
  void *data;
} OBJ_t;

typedef struct{
  int dirty;
  Uint32 realfr;
  FCMD_t *cmds;
  OBJ_t *objs;
} FRAME_t;


typedef struct{
  float x,y,z;
} V;


typedef struct{
  V pos;
  int client;
  int avatar;
} GHOST_t;

typedef struct{
  V pos;
  int model;
  V vel;
  float jumpvel;
  int ghost;
  char goingl;
  char goingr;
  char goingu;
  char goingd;
  char jumping;
} PLAYER_t;

//externs
extern size_t maxframes;
extern size_t maxobjs;

extern FRAME_t *fr;
extern Uint32 frameoffset; //offset to sync client with server
extern Uint32 metafr; //the frame corresponding to "now"
extern Uint32 surefr; //newest frame we are sure of (i.e. it has all its inputs and has been advanced)
extern Uint32 drawnfr;//most recently rendered frame
extern Uint32 hotfr;  //newest fully advanced frame (it's hot and fresh)
extern Uint32 cmdfr;  //newest frame with cmds inserted (possibly in future)
extern int creatables;

extern SDL_Surface *screen;
extern Uint32 ticks;
extern int me;
extern int console_open;
extern UDPpacket *pkt;

//prototypes
void toggleconsole();
void advance();
int findfreeslot(int frame1);
void clearframebuffer();
void cleanup();
void assert(const char *msg,int val);
V *flexpos(OBJ_t *o);

//frame setters
void setmetafr( Uint32 to);
void setsurefr( Uint32 to);
void setdrawnfr(Uint32 to);
void sethotfr(  Uint32 to);
void setcmdfr(  Uint32 to);
void jogframebuffer(Uint32  newmetafr,Uint32 newsurefr);

#endif

