#include "main.hpp"

bool amLeader;
int id;
pthread_t rec_multi_t;
pthread_t rec_uni_t;
pthread_t timeouts_t;

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

	int a = 0;
	sleep(5);
	while(1){
		if(id == 3)
		{
			uni_buff[15] = a++ + '0';
			//multicastDispatcher(uni_buff, strlen((char*)uni_buff));
			//unicastDispatcher(uni_buff, strlen((char*)uni_buff), 2);
		}
		//unicastDispatcher(uni_buff, sizeof(uni_buff), 5);
		//multicastDispatcher(multi_buff, sizeof(multi_buff));
		sleep(2);
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
	if(in_buffer[0] == LEADER_ONLY)
	{
		if(!amLeader)
		{
			return;
		}
		else
		{
			in_buffer[size] = '\0';	// Secured with sizeof(in_buff)-1
			printf(">>Received from (%d) multicast: %s (%d bytes)<<\n\n", id, in_buffer, size);
			unicastDispatcher(in_buffer, size, id);
		}
	}
	else
	{
		printf(">>Received from (%d) multicast: %s (%d bytes)<<\n\n", id, in_buffer, size);
	}
}

void handleUnicast(byte* in_buffer, uint16_t size, int id){
	in_buffer[size] = '\0';	// Secured with sizeof(in_buff)-1
	printf(">>Received from (%d) unicast: %s (%d bytes)<<\n\n", id, in_buffer, size);
}

