#include "main.hpp"

bool amLeader;
int id;
pthread_t rec_multi_t;
pthread_t rec_uni_t;
pthread_t timeouts_t;

void
clean(int signo)
{
	printf("Shuttind down node %d\n", id);
	pthread_cancel(rec_multi_t);
	pthread_cancel(rec_uni_t);
	pthread_cancel(timeouts_t);
	closeSocket(&(net.uni_s));
	closeSocket(&(net.multi_s));
	exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]){
	int rc;
	int nbytes;
	char inbuf[1024];
	char outbuf[1024] = "ola, como estas :)  \n";

	signal(SIGINT, clean);

	if(argc != 3){
		printf("Usage: %s [ID] [LEADER?=Y|N]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	id = atoi(argv[1]);
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
	while(1){
		//unicastDispatcher(uni_buff, sizeof(uni_buff), atoi(argv[2]));
		//multicastDispatcher(multi_buff, sizeof(multi_buff));
		sleep(1);
	}


	/*
	s = &(net.uni_sd);
	printf("ID: %d\n", id);
	outbuf[19] = id % 10 + '0';
	while(1){
		// Listen unicast
		do{
		}while(nbytes > 0);

		// Write unicast
		sleep(1);
	}
	exit(EXIT_SUCCESS);

	while(1){
		// Listen multicast
		do{
		}while(nbytes > 0);

		// Write multicast
		sleep(1);
	}
	*/
}

void handleMulticast(byte* in_buffer, uint16_t size){
	if(in_buffer[0] == LEADER_ONLY && !amLeader)
	{
		return;
	}
	in_buffer[size] = '\0';	// Secured with sizeof(in_buff)-1
	printf(">>Received from multicast: %s<<\n\n", in_buffer);
}

void handleUnicast(byte* in_buffer, uint16_t size){
	in_buffer[size] = '\0';	// Secured with sizeof(in_buff)-1
	printf(">>Received from unicast: %s<<\n\n", in_buffer);
}

