
#ifndef SPARTOR_MAIN_H_
#define SPARTOR_MAIN_H_

#include "SDL.h"
#include "SDL_net.h"
#include "SDL_image.h"

#define VERSION "0.1n?"

#define TICKSAFRAME 30

//macros
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define SWAP(a,b) {a ^= b;b ^= a;a ^= b;}
#define TRY do{
#define HARDER }while(0);
#define HAS(v,flags) (((v)&(flags)) == (flags))

#define assert(expr) { if(!(expr)) SJC_Write( "%s(%d) Assert failed! %s", __FILE__, __LINE__, #expr ); }

//obj flags
#define OBJF_POS  (1<< 0) //has position
#define OBJF_VEL  (1<< 1) //has velocity
#define OBJF_VIS  (1<< 2) //is visible
#define OBJF_HULL (1<< 3) //has a hull
#define OBJF_CLIP (1<< 4) //clips against solids
#define OBJF_PLAT (1<< 5) //acts as a platform
#define OBJF_PVEL (1<< 6) //has player-controlled velocity
#define OBJF_DEL  (1<< 7) //object is marked for deletion
#define OBJF_BNDX (1<< 8) //clips against context edge X-wise
#define OBJF_BNDT (1<< 9) //clips against context top edge
#define OBJF_BNDB (1<<10) //clips against context bottom edge

//cmd flags
#define CMDF_NEW  (1<< 0) //new client connect
#define CMDF_QUIT (1<< 1) //disconenct

//CB flags
#define CBF_SOLID (1<< 0) //solid
#define CBF_PLAT  (1<< 1) //platform
#define CBF_VIS   (1<< 2) //is visible
#define CBF_NULL  (1<< 3) //(dmap only) delta data not present


typedef struct{
  char      cmd;
  char      mousehi;
  char      mousex;
  char      mousey;
  short     flags;
} FCMD_t;

typedef struct{
  short     type;
  short     flags;
  int       context;
  size_t    size;
  void     *data;
} OBJ_t;

typedef struct{
  int       dirty;
  Uint32    realfr;
  FCMD_t   *cmds;
  OBJ_t    *objs;
} FRAME_t;


typedef struct{
  float     x,y,z;
} V;


// map structures //
typedef struct{
  short     flags;
  char      data[CBDATASIZE];
} CB;

typedef struct{
  int       blocksize;
  int       x,y,z;
  CB       *map;
  CB       *dmap;
} CONTEXT_t;


//externs
extern Uint32 ticksaframe;
extern size_t maxframes;
extern size_t maxobjs;
extern int    maxclients;

extern FRAME_t *fr;
extern Uint32 frameoffset; //offset to sync client with server
extern Uint32 metafr; //the frame corresponding to "now"
extern Uint32 surefr; //newest frame we are sure of (i.e. it has all its inputs and has been advanced)
extern Uint32 drawnfr;//most recently rendered frame
extern Uint32 hotfr;  //newest fully advanced frame (it's hot and fresh)
extern Uint32 cmdfr;  //newest frame with cmds inserted (possibly in future)

extern SDL_Surface *screen;
extern Uint32 scale;
extern Uint32 ticks;
extern int    me;
extern int    console_open;

extern Uint32 total_time;
extern Uint32 idle_time;
extern Uint32 render_time;
extern Uint32 adv_move_time;
extern Uint32 adv_collide_time;
extern Uint32 adv_game_time;
extern Uint32 adv_frames;

extern int    eng_realtime;

//prototypes
void toggleconsole();
void advance();
int  findfreeslot(int frame1);
void clearframebuffer();
void cleanup();
void *flex(OBJ_t *o,Uint32 part);

//frame setters
void setmetafr( Uint32 to);
void setsurefr( Uint32 to);
void setdrawnfr(Uint32 to);
void sethotfr(  Uint32 to);
void setcmdfr(  Uint32 to);
void jogframebuffer(Uint32 newmetafr,Uint32 newsurefr);

#endif

