#include "network.hpp"

void* listener(void* _sock){
	sock* s;
	uint16_t nbytes;
	byte inbuf[MAX_TRANSFER+1];
	socklen_t addrlen;
	int received_id;
	char sender_address[INET6_ADDRSTRLEN];

	s = (sock*)_sock;

	while(1){
		addrlen = sizeof(s->in_addr);

		nbytes = recvfrom(s->sd,
			inbuf,
			sizeof(inbuf)-1,			// -1 to temporarily accomodate a \0
			0,
			(struct sockaddr *) &(s->in_addr),
			&addrlen
		);

		if(testLostPacket(inbuf, nbytes)){
			continue;
		}

		if(nbytes == sizeof(inbuf)-1){
			printf("GOT MAXIMUM SIZE UDP PACKET, THIS MAY BE PROBLEMATIC\n");
		}

		s->in_addr.sin_addr.s_addr;
		if(getnameinfo((struct sockaddr*)&(s->in_addr), addrlen, sender_address, sizeof(sender_address), 0, 0, NI_NUMERICHOST) != 0){
			perror("getnameinfo");
			continue;
		}
		//printf("Got %d bytes\n", nbytes-1);

		// Ignoring multicast sent by themselves
		if(!strcmp(sender_address, s->interface_addr) || !strcmp(sender_address, "0.0.0.0")){
			continue;
		}

		if(nbytes > 0){
			received_id = atoi(strrchr(sender_address, '.')+1);
			// Decide type of message handle
			switch(inbuf[0])
			{
				case PAXOS:
					inbuf[nbytes] = '\0';
					s->receiveHandle(inbuf+1, nbytes-1, received_id);
					break;
				case LEADER_ELECTION:
					leaderHandle(inbuf+1, nbytes-1, received_id);
					break;
			}
		}
	}
	return NULL;
}

void dispatcher(sock* s, byte* _out_buffer, uint16_t size, MTI msg_id){
	static bool hassent = false;
	late_message* msg;
	byte* out_buffer;
	int rc;

	out_buffer = (byte*)malloc(sizeof(byte)*size+1);
	out_buffer[0] = msg_id;
	memcpy(out_buffer+1, _out_buffer, size);

	msg = (late_message*)malloc(sizeof(late_message));
	msg->s = s;
	msg->buff = out_buffer;
	msg->size = size+1;

	pthread_t id;
	if (rc = pthread_create(&id,
							NULL,
							sendMessage,
							(void *)msg)){
		printf("Error: Unable to create message thread, %d\n", rc);
		exit(EXIT_FAILURE);
	}
	
}
void multicastDispatcher(byte* out_buffer, uint16_t size, MTI msg_id){
	dispatcher(&(net.multi_s), out_buffer, size, msg_id);
}

void unicastDispatcher(byte* out_buffer, uint16_t size, uint8_t target_id, MTI msg_id){
	setupUnicastClient(&(net.uni_s), target_id);
	dispatcher(&(net.uni_s), out_buffer, size, msg_id);
}

void setupMulticast(sock* s){
	startSocket(s);
	bindMulticastServer(s);
	startMulticastClient(s);
}

void initNetwork(int id){
	char addr[16];
   	sprintf(net.multi_s.interface_addr, "%s.%d", INTERFACE_BASE_IP, id);
   	sprintf(net.uni_s.interface_addr, "%s.%d", INTERFACE_BASE_IP, id);
	printf("ADDRESS %s\n", addr);
	setupMulticast(&(net.multi_s));
	setupUnicast(&(net.uni_s), id);
	net.id = id;
}
