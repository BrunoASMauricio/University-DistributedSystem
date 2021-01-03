#include "main.hpp"

bool amLeader;
int id;
int node_amm;
pthread_t rec_multi_t;
pthread_t rec_uni_t;
pthread_t timeouts_t;

struct new_no n;
int lider_id;

int main(int argc, char *argv[]){
	int rc;
	int nbytes;
	char inbuf[1024];
	char outbuf[1024] = "ola, como estas :)  \n";

	signal(SIGINT, clean);
	srand(getpid());

	if(argc != 4){
		printf("Usage: %s [ID] [LEADER?=Y|N] [NODE AMMOUNT]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	id = atoi(argv[1]);
	node_amm = atoi(argv[3]);

	if(argv[2][0] == 'Y')
	{
		amLeader = true;
		printf("%d starting as leader\n", id);
	}
	else
	{
		amLeader = false;
		printf("%d not starting as leader\n", id);
	}

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

	if (rc = pthread_create(&timeouts_t,
							NULL,
							timeoutHandler,
							NULL)){
		printf("Error: Unable to create thread, %d\n", rc);
		exit(EXIT_FAILURE);
	}
	
	byte uni_buff[17] = "single hi from X";
	uni_buff[15] = id + '0';
	byte  multi_buff[18] = "multi__ hi from X";
	multi_buff[14] = id + '0';

	int role;
	if(amLeader){
		sleep(5);
		role = PROPOSER ;
	}
	else{
		role = ACEPTOR;
	}

	
	//2 lider id (change after)
	lider_id = 2;
	
	n = innit_node(role, lider_id ,id,WINDOW_SIZE,node_amm );
	Paxos_logic( (void *)&n) ;
	while(1){
		
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
	
	printf(">>------Received from (%d) multicast: %s (%d bytes)<<\n\n", id, in_buffer, size);
	struct transition tr = receive_message_paxos_new ((char *)in_buffer);

	
			
	n.paxosStates[tr.decisionNumber] = update_decision_state_new(tr,n.paxosStates[tr.decisionNumber],n);
	print_state(n.paxosStates[tr.decisionNumber]);

}

void handleUnicast(byte* in_buffer, uint16_t size, int id){
	
	printf(">>-------------Received from (%d) unicast: %s (%d bytes)<<\n\n", id, in_buffer, size);

	
	struct transition tr = receive_message_paxos_new ((char *)in_buffer);
	n.paxosStates[tr.decisionNumber] = update_decision_state_new(tr,n.paxosStates[tr.decisionNumber],n);
	print_state(n.paxosStates[tr.decisionNumber]);
}

