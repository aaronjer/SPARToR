
#ifndef __NET_H__
#define __NET_H__


extern UDPsocket hostsock;
extern UDPsocket clientsock;
extern IPaddress ipaddr;


int ip2num(const char *ip);

#endif

