
#ifndef __HOST_H__
#define __HOST_H__


#include "SDL.h"
#include "SDL_net.h"


typedef struct{
  IPaddress addr;
  int connected;
  int ghost; //unused, actually
} CLIENT_t;


void host_start(int port);
void host_stop();
void host();
void host_welcome();

#endif

