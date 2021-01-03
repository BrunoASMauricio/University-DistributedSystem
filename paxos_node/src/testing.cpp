#include "testing.hpp"

bool testLostPacket(byte* buf, uint16_t nbytes){
	float a = (((float)rand()) + ((float)1.0f))/((float)RAND_MAX);
	if(BIT_ERROR_CHANCE > a){
		printf("Lost message\n");
		return true;
	}
	return false;
}
