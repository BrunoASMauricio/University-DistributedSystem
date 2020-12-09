#ifndef MAIN
#define MAIN

#include <pthread.h>


#include "udp.cpp"
#include "network.cpp"


void handleMulticast(byte* in_buffer, uint16_t size);

void handleUnicast(byte* in_buffer, uint16_t size);

#endif
