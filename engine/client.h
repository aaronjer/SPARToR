
#ifndef SPARTOR_CLIENT_H_
#define SPARTOR_CLIENT_H_


extern UDPsocket  clientsock;


void client_start(const char *hostname,int port,int clientport);
void client_stop();
void client();

#endif

