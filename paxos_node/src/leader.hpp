#ifndef leader
#define leader

#include "network.hpp"
#include <unistd.h>

// Decide leader through bully leader election
// Smaller id is strongest
void decideLeader(int node_id);


#endif
