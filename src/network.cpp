#include "network.hpp"

void* listener(void* _sock){
	uint16_t nbytes;
	sock* s;
	byte inbuf[1024];
	socklen_t addrlen;

	s = (sock*)_sock;

	while(1){
		nbytes = recvfrom(s->sd,
			inbuf,
			sizeof(inbuf),
			0,
			(struct sockaddr *) &(s->in_addr),
			&addrlen
		);
		printf("nbytes: %d\n", nbytes);
		if(nbytes > 0){
			//
			// DO STUFF HERE
			//
			s->receiveHandle(inbuf, nbytes);
			inbuf[nbytes] = '\0';
			printf("Got > %s\n", inbuf);
		}
	}
	return NULL;
}

void dispatcher(sock* s, byte* out_buffer, uint16_t size){
	int nbytes;
	nbytes = sendto(
		s->sd,
		out_buffer,
		size,
		0,
		(struct sockaddr*) &(s->out_addr),
		sizeof(s->out_addr)
	);
	if (nbytes < 0) {
		perror("dispatcher sendto");
		return;
	}
}
void multicastDispatcher(byte* out_buffer, uint16_t size){
	dispatcher(&(net.multi_s), out_buffer, size);
}

void unicastDispatcher(byte* out_buffer, uint16_t size){
	dispatcher(&(net.uni_s), out_buffer, size);
}

void setupMulticast(sock* s){
	startSocket(s);
	bindMulticastServer(s);
	startMulticastClient(s);
}

void initNetwork(int id){
	setupMulticast(&(net.multi_s));
	setupUnicast(&(net.uni_s), id);
	net.id = id;
}
