
#ifndef MCDIDDY_MOD_H_
#define MCDIDDY_MOD_H_


#define NATIVEW    384
#define NATIVEH    240
#define MODNAME    "mcdiddy"
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


typedef struct {
  char   *filename;
  int     generated;
  GLuint  glname;
} TEX_T;

typedef struct {
  char   name[100];
  int    num;
} SYS_TEX_T;

typedef struct {
  char   name[16];
  int    presscmd;
  int    releasecmd;
} INPUTNAME_t;


extern int TEX_PLAYER;
extern int TEX_WORLD;
extern int TEX_AMIGO;

extern TEX_T     *textures;
extern size_t     tex_count;
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

