#ifndef MAIN
#define MAIN

#include <stdio.h>
#include <unistd.h>
#include "../../paxos_node/src/network.cpp"
#include "../../paxos_node/src/udp.cpp"
#include "../../lib/paxos.cpp"

void clean(int signo);

void handleMulticast(byte* in_buffer, uint16_t size, int id);

void handleUnicast(byte* in_buffer, uint16_t size, int id);

#endif
