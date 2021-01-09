#ifndef leader
#define leader

#include "network.hpp"
#include <unistd.h>

// Network end-to-end RTT in us
#define RTT 200*1000		// 200ms

enum bully_state{
	s_DOWN,
	s_NORMAL,
	s_ELECTION
};


enum LEADER_MSG{
	/*
	 * Challenges a node
	 * |ARE_U_THERE (1B) | ID (1B) |
	 */
	ARE_U_THERE = 0,
	/*
	 * Reply to a challenge
	 */
	YES,
	/*
	 * Sent by leader
	 * Orders a node to stop and go from NORMAL to ELECTION
	 */
	HALT,
	/*
	 * Sent by leader
	 * Sets the new leader and orders the nodeto go from
	 * ELECTION to NORMAL
	 */
	NEW_LEADER,
	/*
	 * Sent by leader
	 * Non-leaders use itto check if leader is still alive
	 * Inquires node about its' state (checks for new nodes)
	 */
	ARE_U_NORMAL,
	/*
	 * Answer to the ARE-U-NORMAL
	 */
	STATE,
};

typedef struct{
	int leader_id;
	int node_id;
	bool am_strongest;						// If true, haven't backedoff
	byte message[6][10];
	byte sizes[6] = {2,1,1,2,1,2};
	bully_state st;
	pthread_mutex_t lock;
	unsigned long int timeout;
}election;


election el;


bool
/*
 * Performs phase 1 of bully leader election
 * Sends challenges and may or may not back off
 * Returns am_strongest
 */
phase1();

void
/*
 * Setup bully leader election
 */
setupBully(int node_id);

void
/*
 * Decide leader through bully leader election
 * Smaller id is strongest
 *
 * Leader election messages
*/
decideLeader();

void
/*
 * The handler for leader messages
 * Implements the bully state machine
 * Not checking for non-byzantine faults:
 * 	f.ex.	Can't receive a YES without having sent an ARE_U_THERE
 * 			Can't receive a STATE without having sent an ARE_U_NORMAL
 */
leaderHandle(byte* msg, int size, int id);


void
/*
 * Checks if a new election is starting
 */
election_check();


#endif
