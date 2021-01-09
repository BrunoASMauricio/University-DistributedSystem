#include "leader.hpp"

void setupBully(int node_id){
	el.node_id = node_id;
	el.am_strongest = true;
	el.leader_id = -1;
	el.st = s_ELECTION;
	el.message[ARE_U_THERE][0] = ARE_U_THERE;
	el.message[ARE_U_THERE][1] = node_id;
	el.message[YES][0] = YES;
	el.message[HALT][0] = HALT;
	el.message[NEW_LEADER][0] = NEW_LEADER;
	el.message[NEW_LEADER][1] = node_id;
	el.message[ARE_U_NORMAL][0] = ARE_U_NORMAL;
	el.message[STATE][0] = STATE;
	//el.message[STATE][1] = ;
	if (pthread_mutex_init(&(el.lock), NULL) != 0){
        perror("mutex init failed for outbound lock\n");
    }
	decideLeader();
}

bool phase1(){
	printf("Starting phase 1\n");
	el.st = s_ELECTION;
	multicastDispatcher(el.message[ARE_U_THERE], el.sizes[ARE_U_THERE], LEADER_ELECTION);
	usleep(2*RTT);
	return el.am_strongest;
}

void phase2(){
	printf("Starting phase 2\n");
	usleep(2*RTT);
	multicastDispatcher(el.message[HALT], el.sizes[HALT], LEADER_ELECTION);
	usleep(RTT);
	multicastDispatcher(el.message[NEW_LEADER], el.sizes[NEW_LEADER], LEADER_ELECTION);
	el.st = s_NORMAL;
	el.leader_id = el.node_id;
}

void decideLeader(){
	do{
		printf("Deciding leader\n");
		// Wait for possible ARE_U_NORMAL
		usleep(2*RTT);
		// Smallest IDs start first
		// not really necessary
		// just speeds decision up since it decreases the collision posibility
		usleep(el.node_id*el.node_id*el.node_id*1000);
		// Current leader already started election?
		printf("Am strongest? %d\n", el.am_strongest);
		if(el.am_strongest && phase1()){
			phase2();
			return;
		}
		// Wait for leader to be elected
		printf("Waiting for leader\n");
		usleep(3*RTT);	
		// What if leader isn't elected??
	}while(el.leader_id == -1);
}

void leaderHandle(byte* msg, int size, int id){
	byte answer[MAX_TRANSFER];
	pthread_mutex_lock(&(el.lock));
	switch(msg[0]){
		case ARE_U_THERE:
			printf("Got ARE_U_THERE %d %d\n", el.node_id > id, el.am_strongest);
			// If stronger than the challenging node AND haven't
			// backed off (there isn't a stronger node)
			// reply YES and start a new election
			if(el.node_id > id && el.am_strongest){
				unicastDispatcher(el.message[YES], el.sizes[YES], id, LEADER_ELECTION);
				el.st = s_ELECTION;
			}else{
				el.am_strongest = false;
			}
			break;

		case YES:
			printf("Got YES\n");
			// Backing off
			el.st = s_NORMAL;
			el.am_strongest = false;
			break;

		case HALT:
			printf("Got HALT\n");
			el.st = s_ELECTION;
			break;

		case NEW_LEADER:
			printf("Got NEW_LEADER %d\n", msg[1]);
			el.leader_id = msg[1];
			el.st = s_NORMAL;
			break;

		case ARE_U_NORMAL:
			printf("Got ARE_U_NORMAL\n");
			el.message[STATE][1] = el.st;
			unicastDispatcher(el.message[STATE], el.sizes[STATE], id, LEADER_ELECTION);
			break;

		case STATE:
			printf("Got STATE\n");
			if(msg[1] != s_NORMAL){
				el.st = s_ELECTION;
			}
			break;
	}
	pthread_mutex_unlock(&(el.lock));
}

void election_check(){
	if(el.st == s_ELECTION){
		decideLeader();
	}
}
