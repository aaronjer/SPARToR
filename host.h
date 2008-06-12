
#ifndef __HOST_H__
#define __HOST_H__


#include "SDL/SDL.h"
#include "SDL/SDL_net.h"


typedef struct{
  IPaddress addr;
  int ghost;
} CLIENT_t;


void host_start(int port);
void host_stop();
void host();
void host_welcome();

#endif

