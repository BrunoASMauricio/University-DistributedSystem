#ifndef NETWORK
#define NETWORK

#include "udp.hpp"
typedef struct{
	sock uni_s;						// Unicast socket
	sock multi_s;					// Multicast socket
	int id;							// Network ID
}network;


void*
/*
 * Thread that listens to multicast
 * communications
 */
multicastListener(void* dummy);

void*
/*
 * Thread that listens to unicast
 * communications
 */
unicastListener(void* dummy);

/*
 * Sets up a socket for multicast input and output
 */
void setupMulticast(sock* s);

/*
 * Sets up a socket for unicast input and output
 */
void setupUnicast(sock* s, int id);



network net;



#endif
