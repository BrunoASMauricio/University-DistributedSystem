#include "main.hpp"

bool amLeader;
int id;
int node_amm;
pthread_t rec_multi_t;
pthread_t rec_uni_t;
pthread_t timeouts_t;

struct new_no n;

int main(int argc, char *argv[]){
	int rc;
	int nbytes;
	
	signal(SIGINT, clean);
	srand(getpid());

	if(argc != 3){
		printf("Usage: %s [ID] [NODE AMMOUNT]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	id = atoi(argv[1]);
	node_amm = atoi(argv[2]);
	
	printf("Starting with id %d, node ammount %d\n", id, node_amm);

	initNetwork(id);
	net.multi_s.receiveHandle = &handleMulticast;
	net.uni_s.receiveHandle = &handleUnicast;

	if (rc = pthread_create(&rec_multi_t,
							NULL,
							listener,
							&(net.multi_s))){
		printf("Error: Unable to create thread, %d\n", rc);
		exit(EXIT_FAILURE);
	}
	if (rc = pthread_create(&rec_uni_t,
							NULL,
							listener,
							&(net.uni_s))){
		printf("Error: Unable to create thread, %d\n", rc);
		exit(EXIT_FAILURE);
	}

	
	byte uni_buff[17] = "single hi from X";
	uni_buff[15] = id + '0';
	byte  multi_buff[18] = "multi__ hi from X";
	multi_buff[14] = id + '0';

	setupBully(id);
	printf("Leader: %d\n", el.leader_id);

	int role;
	if(el.leader_id == el.node_id){
		role = PROPOSER;
		printf("Beggining Proposer\n");
	}
	else{
		role = ACEPTOR;
		printf("Beggining Aceptor\n");
	}

	n = innit_node(role, el.leader_id, id, WINDOW_SIZE, node_amm );

    print_node(n);

	/*
	pthread_t idt;
	if (rc = pthread_create(&idt,
							NULL,
							Paxos_logic,
							(void *)&n)){
		printf("Error: Unable to create thread, %d\n", rc);
		exit(EXIT_FAILURE);
	}
	*/

	/*
	if (rc = pthread_create(&timeouts_t,
							NULL,
							timeoutHandler,
							(void *)&n)){
		printf("Error: Unable to create thread, %d\n", rc);
		exit(EXIT_FAILURE);
	}*/


	//Paxos_logic( (void *)&n) ;
	while(1){
		Paxos_logic(&n);
		election_check();
	}

}

void
clean(int signo)
{
	printf("Shutting down node %d\n", id);
	pthread_cancel(rec_multi_t);
	pthread_cancel(rec_uni_t);
	pthread_cancel(timeouts_t);
	closeSocket(&(net.uni_s));
	closeSocket(&(net.multi_s));
	exit(EXIT_SUCCESS);
}

void handleMulticast(byte* in_buffer, uint16_t size, int id){
	
	//printf(">>------Received from (%d) multicast: %s (%d bytes)<<\n\n", id, in_buffer, size);
	struct transition tr = receive_message_paxos_new ((char *)in_buffer);

	pthread_mutex_lock(&(n.lock)); 
	if(tr.decisionNumber <= n.lastPhase1innit && tr.decisionNumber > n.lastPhase1complete ){		
		n.paxosStates[tr.decisionNumber] = update_decision_state_new(tr,n.paxosStates[tr.decisionNumber],n);
		print_state(n.paxosStates[tr.decisionNumber]);
	}
	pthread_mutex_unlock(&(n.lock)); 
}

void handleUnicast(byte* in_buffer, uint16_t size, int id){
	
	//printf(">>-------------Received from (%d) unicast: %s (%d bytes)<<\n\n", id, in_buffer, size);

	
	struct transition tr = receive_message_paxos_new ((char *)in_buffer);

	pthread_mutex_lock(&(n.lock)); 

	if(tr.decisionNumber <= n.lastPhase1innit && tr.decisionNumber > n.lastPhase1complete ){
		n.paxosStates[tr.decisionNumber] = update_decision_state_new(tr,n.paxosStates[tr.decisionNumber],n);
		print_state(n.paxosStates[tr.decisionNumber]);
	}

	pthread_mutex_unlock(&(n.lock)); 
}

