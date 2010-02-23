
#ifndef SPARTOR_NET_H_
#define SPARTOR_NET_H_


#define PROTONAME "SPARToR Protocol"
#define HOSTPORT 31103
#define CLIENTPORT 31109

extern UDPsocket  hostsock;
extern UDPsocket  clientsock;
extern IPaddress  ipaddr;
extern int        maxclients;
extern UDPpacket *pkt;


void   disconnect();
Uint8 *packframe(      Uint32 packfr,size_t *n);
Uint8 *packframecmds(  Uint32 packfr,size_t *n);
int    unpackframe(    Uint32 packfr,Uint8  *data, size_t  len);
int    unpackframecmds(Uint32 packfr,Uint8  *data, size_t  len);
void   packbytes(      Uint8 *data,  Uint64  value,size_t *offset,int width);
Uint64 unpackbytes(    Uint8 *data,  size_t  len,  size_t *offset,int width);
void   inspectbytes(   Uint8 *data,  int     n);

#endif

