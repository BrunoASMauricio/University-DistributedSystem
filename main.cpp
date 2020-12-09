#include "udp.cpp"
#include "network.cpp"


int main(int argc, char *argv[]){
	int id;
	pthread_t rec_multi;
	pthread_t rec_uni;
	pthread_t timeouts;
	sock* s;
	int nbytes;
	char inbuf[1024];
	char outbuf[1024] = "ola, como estas :)  \n";

	if(argc != 3){
		printf("Usage: %s [ID] [TARGET]\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	id = atoi(argv[1]);
	initNetwork(id);
	/*
	s = &(net.uni_sd);
	setupUnicastClient(s, atoi(argv[2]));
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

}

void handleUnicast(byte* in_buffer, uint16_t size){

}

