#ifndef test_hpp
#define test_hpp

#include "udp.hpp"
#include "../../lib/paxos.hpp"


#define BIT_ERROR_CHANCE 0

bool testLostPacket(byte* buf, uint16_t nbytes);

void writeToFile(char* string, int id);

void* sendMessage(void* _msg);

#endif
