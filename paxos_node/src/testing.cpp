#include "testing.hpp"

bool testLostPacket(float error_percentage){
	float a = (((float)rand()) + ((float)1.0f))/((float)RAND_MAX);
	if(error_percentage > a){
		//printf("Lost message\n");
		return true;
	}
	return false;
}

void writeToFile(char* string, int id){
	static FILE* fp = NULL;
	char filename[10];
	if(!fp){
		snprintf(filename, sizeof(filename), "./node_%d", id);
		fp = fopen(filename, "a");
		if(!fp){
			perror("fopen node_..");
			return;
		}
	}
	fwrite(string, sizeof(char), strlen(string), fp);
	fflush(fp);
}

/*
struct paxos_state{
    int decisionNumber;
    int state;
    int promMessageVal;
    int promMessageID;

    int currentMessageID ;
    int currentMessageVal;

    int numReceivedPromises;
    int numNodes;
};
*/
/*
while(1){

	paxos_state a;

	a.decisionNumber = 1;
	a.state = 2;
	a.promMessageVal = 3;
	a.promMessageID = 4;

	a.currentMessageID = 5;
	a.currentMessageVal = 6;
	a.numReceivedPromises = 7;
	a.numNodes = -8;

	if(id == 3){
		writeStatusToFile(&a, id);
		int *b = (int*)readStatusFromFile(id);
		for(int i = 0; i < sizeof(paxos_state)/sizeof(int); i++){
			printf("GOT %d\n", b[i]);
		}
	}
	sleep(100);
}
*/
void writeStatusToFile(paxos_state* _st, int id){
	FILE* fp = NULL;
	char filename[17];
	int* st = (int*)_st;

	snprintf(filename, sizeof(filename), "./node_status_%d", id);
	fp = fopen(filename, "w");
	if(!fp){
		perror("fopen node_status_..");
		return;
	}
	for(int i = 0; i < sizeof(paxos_state)/sizeof(int); i++){
		printf("%d\n", i);
		fprintf(fp, "%d\n", st[i]);
	}
	fclose(fp);
}

paxos_state* readStatusFromFile(int id){
	FILE* fp;
	char filename[17];
	paxos_state* _st = (paxos_state*)malloc(sizeof(paxos_state));;
	int* st = (int*)_st;
	
	snprintf(filename, sizeof(filename), "./node_status_%d", id);
	fp = fopen(filename, "r");
	if(!fp){
		perror("fopen node_status_..");
		return NULL;
	}
	for(int i = 0; i < sizeof(paxos_state)/sizeof(int); i++){
		printf("%d\n", i);
		fscanf(fp, "%d\n", &(st[i]));
	}
	fclose(fp);
	return _st;
}

int
range(int min, int max)
{
	//49152 through 65535
	return (rand() % (max - min + 1)) + min;
}

void* sendMessage(void* _msg){
	late_message* msg = (late_message*)_msg;
	int nbytes;

	usleep(range(0,100)*100);	//0 to 10ms

	nbytes = sendto(
		msg->s->sd,
		msg->buff,
		msg->size,
		0,
		(struct sockaddr*) &(msg->s->out_addr),
		sizeof(msg->s->out_addr)
	);
	printf("Sent message (%d/%d bytes)\n", msg->size, nbytes);
	free(msg->buff);
	free(msg);
	if (nbytes < 0) {
		perror("dispatcher sendto");
	}
	return NULL;
}
