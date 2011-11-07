
#ifndef DEADKING_MOD_H_
#define DEADKING_MOD_H_


#define NATIVEW    480
#define NATIVEH    300
#define MODNAME    "deadking"
#define CBDATASIZE 4     //context block (CB) payload size
#define MAXCMDDATA 64    //maximum size of extra command data


#include <GL/glew.h>
#include "SDL.h"
#include "SDL_net.h"
#include "main.h"
#include "console.h"
#include "net.h"
#include "video.h"
#include "input.h"
#include "patt.h"
#include "mod_private.h"
#include "obj_.h"
#include <math.h>


#define COUNTOF(ident) ((sizeof (ident)) / (sizeof *(ident)))

#define TILEX 8  // number of tiles across per tex
#define TILEY 16 //   "        "    down
#define TILEW 30 // width of tiles
#define TILEH 16 // height of tiles
#define TILEUW 32 // "used width" distance between tiles
#define TILEUH 16 // "used height"
#define TILE2NATIVE_X(x,y) (((x)-(y))*TILEUW/2 + 225)
#define TILE2NATIVE_Y(x,y) (((x)+(y))*TILEUH/2      )
#define SHIFTX(x) ((x)-TILEUW/2-225)
#define SHIFTY(y) ((y)             )
#define NATIVE2TILE_X(x,y) ((SHIFTX(x)+SHIFTY(y)*2)/TILEUW) // times 2 b/c tiles are twice as wide as tall
#define NATIVE2TILE_Y(x,y) ((SHIFTY(y)-SHIFTX(x)/2)/TILEUH)

#define TEX_PLAYER 0
#define TEX_WORLD  1
#define TEX_AMIGO  2


extern SYS_TEX_T  sys_tex[];
extern size_t     num_sys_tex;

extern INPUTNAME_t inputnames[];
extern int numinputnames;


extern char objectnames[][16];


extern CB *hack_map; //FIXME REMOVE SOMEDAY
extern CB *hack_dmap;


void mod_setup(Uint32 setupfr);
void mod_recvobj(OBJ_t *o);
void mod_setvideo(int w,int h);
void mod_quit();
void mod_keybind(int device,int sym,int press,char cmd);
int  mod_mkcmd(FCMD_t *c,int device,int sym,int press);
int  mod_command(char *q);
void mod_loadsurfs(int quit);
void mod_predraw(Uint32 vidfr);
void mod_draw(int objid,OBJ_t *o);
void mod_postdraw(Uint32 vidfr);
void mod_outerdraw(Uint32 vidfr,int w,int h);
void mod_adv(int objid,Uint32 a,Uint32 b,OBJ_t *oa,OBJ_t *ob);


#endif

