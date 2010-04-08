
#ifndef SPARTOR_HOST_H_
#define SPARTOR_HOST_H_


#include "SDL.h"
#include "SDL_net.h"


typedef struct{
  IPaddress addr;
  int connected;
  int ghost; //unused, actually
} CLIENT_t;


extern UDPsocket  hostsock;


void host_start(int port);
void host_stop();
void host();
void host_welcome();

#endif

