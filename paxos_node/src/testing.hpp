#ifndef test
#define test

#include "udp.hpp"

#define BIT_ERROR_CHANCE 0

bool testLostPacket(byte* buf, uint16_t nbytes);

void writeToFile(char* string, int id);



#endif
