#include "testing.hpp"

bool testLostPacket(byte* buf, uint16_t nbytes){
	float a = (((float)rand()) + ((float)1.0f))/((float)RAND_MAX);
	if(BIT_ERROR_CHANCE > a){
		printf("Lost message\n");
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
