#ifndef NETWORK
#define NETWORK

#include "testing.hpp"
#include "../../lib/paxos.hpp"
#include "udp.hpp"
#include "leader.hpp"

// Delay to time out in ns
#define PROMISE_TIMEOUT_PERIOD 10E9

enum timeouts{
   	PROMISE,
};

typedef struct{
	int timeouts;
	pthread_mutex_t lock;
	unsigned long int promise_timeout;
}timeout_schedules;

typedef struct{
	sock uni_s;						// Unicast socket
	sock multi_s;					// Multicast socket
	int id;							// Network ID
	timeout_schedules t_s;			// Timeout Scheduling
}network;

enum MTI{
	PAXOS,
	LEADER_ELECTION
};

// Largest admissible UDP packet
#define MAX_TRANSFER 2048

void*
/*
 * The actual listener
 * First byte (Message Type Identifier) is removed
 */
listener(void* _sock);
	
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

void setupUnicast(sock* s, int id);


void
/*
 * Sends a message
 * Sets the id as the first byte
 */
dispatcher(sock* s, byte* _out_buffer, uint16_t size, MTI id);

void multicastDispatcher(byte* out_buffer, uint16_t size, MTI msg_id);

void unicastDispatcher(byte* out_buffer, uint16_t size, uint8_t target_id, MTI msg_id);

network net;



#endif
