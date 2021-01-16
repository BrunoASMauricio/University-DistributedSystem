#ifndef test_hpp
#define test_hpp

#include "udp.hpp"
#include "../../lib/paxos.hpp"


bool testLostPacket(float error_percentage);

void writeToFile(char* string, int id);

void* sendMessage(void* _msg);



void writeNodeState(struct new_no n);

struct new_no  readNodeState(int id, int *err);


#endif
