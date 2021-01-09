#include "network.hpp"
void* listener(void* _sock){
	sock* s;
	uint16_t nbytes;
	byte inbuf[MAX_TRANSFER];
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
		printf("Got %d bytes\n", nbytes-1);

		// Ignoring multicast sent by themselves
		if(!strcmp(sender_address, s->interface_addr) || !strcmp(sender_address, "0.0.0.0")){
			continue;
		}

		if(nbytes > 0){
			received_id = atoi(strrchr(sender_address, '.')+1);
			// Decide type of message handle
			switch(inbuf[0])
			{
				case NORMAL:
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

void dispatcher(sock* s, byte* _out_buffer, uint16_t size, MTI id){
	static bool hassent = false;
	byte out_buffer[MAX_TRANSFER];
	int nbytes;
	
	memcpy(out_buffer+1, _out_buffer, size);
	out_buffer[0] = id;
	
	nbytes = sendto(
		s->sd,
		out_buffer,
		size,
		0,
		(struct sockaddr*) &(s->out_addr),
		sizeof(s->out_addr)
	);
	printf("Sent message (%d/%d bytes)\n", out_buffer, size, nbytes);
	if (nbytes < 0) {
		perror("dispatcher sendto");
		return;
	}
}
void multicastDispatcher(byte* out_buffer, uint16_t size){
	dispatcher(&(net.multi_s), out_buffer, size, NORMAL);
}

void unicastDispatcher(byte* out_buffer, uint16_t size, uint8_t target_id){
	setupUnicastClient(&(net.uni_s), target_id);
	dispatcher(&(net.uni_s), out_buffer, size, NORMAL);
}

void setupMulticast(sock* s){
	startSocket(s);
	bindMulticastServer(s);
	startMulticastClient(s);
}

void initNetwork(int id){
	strcpy(net.multi_s.interface_addr, INTERFACE_BASE_IP);
	net.multi_s.interface_addr[strlen(net.multi_s.interface_addr)-1] = '0'+id;
	strcpy(net.uni_s.interface_addr, INTERFACE_BASE_IP);
	net.uni_s.interface_addr[strlen(net.uni_s.interface_addr)-1] = '0'+id;
	setupMulticast(&(net.multi_s));
	setupUnicast(&(net.uni_s), id);
	net.id = id;
}
