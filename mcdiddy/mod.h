
#ifndef MCDIDDY_MOD_H_
#define MCDIDDY_MOD_H_


#define NATIVEW    384
#define NATIVEH    240
#define MODNAME    "mcdiddy"
#define CBDATASIZE 4     //context block (CB) payload size


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
  char name[16];
  int  presscmd;
  int  releasecmd;
} INPUTNAME_t;


enum { TEX_PLAYER = 0,
       TEX_WORLD,
       TEX_AMIGO,
       TEX_COUNT };
extern GLuint textures[TEX_COUNT];


extern int numinputnames;
extern INPUTNAME_t inputnames[];
extern char objectnames[][16];


void mod_setup(Uint32 setupfr);
void mod_recvobj(OBJ_t *o);
void mod_setvideo(int w,int h);
void mod_quit();
void mod_keybind(int device,int sym,int press,char cmd);
char mod_key2cmd(int device,int sym,int press);
int  mod_command(char *q);
void mod_loadsurfs(int quit);
void mod_predraw(SDL_Surface *screen,Uint32 vidfr);
void mod_draw(SDL_Surface *screen,int objid,OBJ_t *o);
void mod_adv(int objid,Uint32 a,Uint32 b,OBJ_t *oa,OBJ_t *ob);


#endif

