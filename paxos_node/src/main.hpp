#ifndef MAIN
#define MAIN

#include <pthread.h>
#include <signal.h>

#include "../../lib/paxos.cpp"
#include "udp.cpp"
#include "network.cpp"
#include "testing.cpp"


void handleMulticast(byte* in_buffer, uint16_t size, int id);

void handleUnicast(byte* in_buffer, uint16_t size, int id);

void clean(int signo);


#endif
