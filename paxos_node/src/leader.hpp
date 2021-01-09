#ifndef leader
#define leader

#include "network.hpp"
#include <unistd.h>

/* Decide leader through bully leader election
 * Smaller id is strongest
 *
 * Leader election messages
*/
void decideLeader(int node_id);

void
/*
 * The handler for leader messages
 */
leaderHandle(byte* msg, int size, int id);

#endif
