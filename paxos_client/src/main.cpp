#include "main.hpp"

int id;

int main(int argc, char *argv[]){
	char user_input[100];
	byte* msg;
	if(argc < 2)
	{
		printf("Usage: %s ID=last_node_id+1\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	id = atoi(argv[1]);

	printf("Starting client with id %d\n", id);

	strcpy(net.multi_s.interface_addr, INTERFACE_BASE_IP);
	net.multi_s.interface_addr[strlen(net.multi_s.interface_addr)-1] = '0'+id;
	net.multi_s.receiveHandle = &handleMulticast;

	setupMulticast(&(net.multi_s));

	sleep(2);
	char treta[100] = "WARM UP WHY";
	multicastDispatcher((byte*)treta, strlen(treta));

	while(1){
		printf("Enter a string to send: ");
		if(!fgets(user_input+1, sizeof(user_input)-1, stdin))
		{
			printf("Could not read input, please try again\n");
			continue;
		}
		user_input[0] = LEADER_ONLY;
		// Cut the \n
		user_input[strlen(user_input)-1] = '\0';
		
		multicastDispatcher((byte*)user_input, strlen(user_input+1)+1);
	}
}

void
clean(int signo)
{
	printf("Shuttind down node %d\n", id);
	closeSocket(&(net.multi_s));
	exit(EXIT_SUCCESS);
}


void handleMulticast(byte* in_buffer, uint16_t size){
	in_buffer[size] = '\0';	// Secured with sizeof(in_buff)-1
	printf(">>Received from multicast: %s<<\n\n", in_buffer);
}

