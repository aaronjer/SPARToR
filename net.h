
#ifndef __NET_H__
#define __NET_H__


#define PROTONAME "SPARToR Protocol"
#define PROTOVERS "0.9a"
#define HOSTPORT 31103
#define CLIENTPORT 31109

extern UDPsocket hostsock;
extern UDPsocket clientsock;
extern IPaddress ipaddr;


void disconnect();

#endif

