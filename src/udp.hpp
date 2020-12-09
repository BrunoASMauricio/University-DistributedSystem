#ifndef UDP
#define UDP

#include <string.h>
#define SETBIT(bit,byte) byte |= (1 << bit)
#define CLEARBIT(bit,byte) byte &= ~(1 << bit)
#define CHECKBIT(bit,byte) (byte) & (1 << bit)

#define MULTICAST_GROUP "239.255.0.1"
#define INTERFACE_BASE_IP "127.0.0.0"

#define MULTICAST_PORT 9999
#define UNICAST_PORT 9998

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>	

typedef uint8_t byte;

typedef struct{
	int sd;									// Socket descriptor
	char interface_addr[16];				// Interface IP
	struct sockaddr_in in_addr;				// Input sockaddress
	struct sockaddr_in out_addr;			// Output sockaddress
	void (*receiveHandle)(byte*, uint16_t);	// The funcitonthat handles input
}sock;

/*
 * Basic socket setup
 */
void startSocket(sock* s);

/*
 * Sets respective socket for
 * blocking or non blocking according
 * to set_block
 */
void socketBlocking(sock* s, bool set_block);

// --------- Multicast ---------
/*
 * Binds a socket to the MULTICAST group and port
 * Sets the port to be reusable
 * Restricts the port for only 1 multicast stream
 * Enables loopback
 */
void bindMulticastServer(sock* s);

/*
 * Sets up the socket output address
 * for a multicast client
 */
void startMulticastClient(sock* s);



// --------- Unicast ---------

/*
 * Binds a socket to a loopback interface on the UNICAST port
 * Sets the port to be reusable
 */
void bindUnicastServer(sock* s);

/*
 * Sets up the socket output address
 * for unicast transmission to the target_addr
 */
void setupUnicastClient(sock* s, int target_addr);






#endif
