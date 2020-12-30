#include "main.hpp"

int id;
pthread_t rec_multi_t;
pthread_t rec_uni_t;

int main(int argc, char *argv[]){
	char user_input[100];
	byte* msg;
	int rc;

	if(argc < 2)
	{
		printf("Usage: %s ID=last_node_id+1\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	id = atoi(argv[1]);

	printf("Starting client with id %d\n", id);

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

	while(1){
		printf("Enter a string to send:\n");
		if(!fgets(user_input+1, sizeof(user_input)-1, stdin))
		{
			printf("Could not read input, please try again\n");
			continue;
		}

		user_input[0] = LEADER_ONLY;
		// Cut the \n
		user_input[strlen(user_input)-1] = '\0';
		
		if(strlen(user_input+1) == 0)
		{
			continue;
		}

		multicastDispatcher((byte*)user_input, strlen(user_input+1)+1);
	}
}

void
clean(int signo)
{
	printf("Shutting down node %d\n", id);
	pthread_cancel(rec_multi_t);
	pthread_cancel(rec_uni_t);
	closeSocket(&(net.multi_s));
	exit(EXIT_SUCCESS);
}


void handleMulticast(byte* in_buffer, uint16_t size, int id){
	in_buffer[size] = '\0';	// Secured with sizeof(in_buff)-1
	printf(">>Received from (%d) multicast: %s<<\n\n", id, in_buffer);
}

void handleUnicast(byte* in_buffer, uint16_t size, int id){
	in_buffer[size] = '\0';	// Secured with sizeof(in_buff)-1
	printf(">>Received from (%d) unicast: %s<<\n\n", id, in_buffer);
}
