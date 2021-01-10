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

	initNetwork(id,0);
	net.multi_s.receiveHandle = &handleMulticast;
	net.uni_s.receiveHandle = &handleUnicast;

	if (rc = pthread_create(&rec_uni_t,
							NULL,
							listener,
							&(net.uni_s))){
		printf("Error: Unable to create thread, %d\n", rc);
		exit(EXIT_FAILURE);
	}
		struct transition tr;
		int dec_num;
		int val;
	

		while(1){
		
		printf("Enter a string to send:\n");
		
		if(scanf("%d %d",&val,&dec_num))
		{
				tr.name = CLIENT_MSG;
		
				//obv isto vai mudar mas por enquanto fica
				tr.messageVal = val;
				tr.dstNodeId  = MULTICAST;
				tr.decisionNumber = dec_num ;
				dec_num +=1;

				send_message_paxos_new (tr);
				
		}
		else{
			printf("Could not read input, please try again\n");
			printf("format: [int value] [int decision number]\n");
			int c;
			while((c = getchar()) != '\n' && c != EOF);
			continue;
		}

		
	

		//multicastDispatcher((byte*)user_input, strlen(user_input+1)+1);
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
	printf(">>Received from (%d) multicast: %s (%d bytes)<<\n\n", id, in_buffer, size);
}

void handleUnicast(byte* in_buffer, uint16_t size, int id){
	in_buffer[size] = '\0';	// Secured with sizeof(in_buff)-1
	printf(">>Received from (%d) unicast: %s (%d bytes)<<\n\n", id, in_buffer, size);
}
