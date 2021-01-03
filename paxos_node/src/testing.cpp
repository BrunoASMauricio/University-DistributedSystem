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
	char filename[9];
	if(!fp){
		snprintf(filename, 9, "./node_%d", id);
		fp = fopen(filename, "a");
		if(!fp){
			perror("fopen");
			return;
		}
	}
	fwrite(string, sizeof(char), strlen(string), fp);
}
