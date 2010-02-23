
#ifndef SPARTOR_MOD_H_
#define SPARTOR_MOD_H_

#include "SDL.h"
#include "SDL_net.h"
#include "main.h"

void mod_setup(Uint32 setupfr);
void mod_setvideo(int w,int h);
void mod_quit();
char mod_key2cmd(int sym,int press);
void mod_loadsurfs(int quit);
void mod_predraw(SDL_Surface *screen,Uint32 vidfr);
void mod_draw(SDL_Surface *screen,int objid,OBJ_t *o);
void mod_adv(Uint32 objid,Uint32 a,Uint32 b,OBJ_t *oa,OBJ_t *ob);


#endif

