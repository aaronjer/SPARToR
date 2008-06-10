
#include "SDL/SDL.h"
#include "SDL/SDL_net.h"
#include <stdlib.h>
#include "net.h"

UDPsocket hostsock = NULL;
UDPsocket clientsock = NULL;
IPaddress ipaddr;

int ip2num(const char *ip){
  int a,b,c,d;
  a = atoi(ip);
  while(*ip)
    if(*ip++=='.') {ip++;break;}
  b = atoi(ip);
  while(*ip)
    if(*ip++=='.') {ip++;break;}
  c = atoi(ip);
  while(*ip)
    if(*ip++=='.') {ip++;break;}
  d = atoi(ip);
  return (a*16777216 + b*65536 + c*256 + d);
}

