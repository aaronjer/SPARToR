

#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include "main.h"
#include "console.h"
#include "net.h"


void mod_adv(int a,int b,OBJ_t *oa,OBJ_t *ob) {
  int i;
  if( ob->type==OBJT_MOTHER ) {
    //the mother object is sort of the root of the object tree only not and
    //there's no object tree
    for(i=0;i<maxclients;i++) {
      if( fr[b].cmds[i].flags )
        SJC_Write("***NEW CLIENT DETECTED***");
    }
  }
}



