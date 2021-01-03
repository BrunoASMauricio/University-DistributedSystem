#include "network.hpp"

void setupTimeout(timeouts t_o){
	timespec Res;
	unsigned long int Act;
	pthread_mutex_lock(&(net.t_s.lock));
	
	clock_gettime(CLOCK_REALTIME, &Res);
	Act = Res.tv_sec * (int64_t)1000000000UL + Res.tv_nsec;
	SETBIT(t_o, net.t_s.timeouts);
	switch(t_o){
		case PROMISE:
			net.t_s.promise_timeout = Act + PROMISE_TIMEOUT_PERIOD;
			break;
	}

	pthread_mutex_unlock(&(net.t_s.lock));
}

void stopTimeout(timeouts t_o){
	pthread_mutex_lock(&(net.t_s.lock));
	CLEARBIT(t_o, net.t_s.timeouts);
	pthread_mutex_unlock(&(net.t_s.lock));
}

void* timeoutHandler(void* dummy){
	timespec Res;
	unsigned long int Act;
	
	while(1){
		clock_gettime(CLOCK_REALTIME, &Res);
		Act = Res.tv_sec * (int64_t)1000000000UL + Res.tv_nsec;
		pthread_mutex_lock(&(net.t_s.lock));
		if(CHECKBIT(PROMISE, net.t_s.timeouts)){
			if(Act > net.t_s.promise_timeout){
				//timeout();
				// DO SOMETHING
			}
		}
		printf("Nothing to timeout\n");
		sleep(10);
	}
}

void* listener(void* _sock){
	sock* s;
	uint16_t nbytes;
	byte inbuf[1024];
	socklen_t addrlen;
	char sender_address[INET6_ADDRSTRLEN];

	s = (sock*)_sock;

	while(1){
		nbytes = recvfrom(s->sd,
			inbuf,
			sizeof(inbuf)-1,			// -1 to temporarily accomodate a \0
			0,
			(struct sockaddr *) &(s->in_addr),
			&addrlen
		);
		printf("Got %d bytes\n", nbytes);
		s->in_addr.sin_addr.s_addr;
		if(getnameinfo((struct sockaddr*)&(s->in_addr), addrlen, sender_address, sizeof(sender_address), 0, 0, NI_NUMERICHOST) != 0){
			perror("getnameinfo");
			continue;
		}
		
		// Ignoring multicast sent by themselves, or with the IGNORE byte
		if(!strcmp(sender_address, s->interface_addr) || !strcmp(sender_address, "0.0.0.0") || inbuf[0] == IGNORE){
			continue;
		}

		//printf("Received %d bytes from %s: %s\n", nbytes, inbuf, sender_address);
		if(nbytes > 0){
			//
			// DO STUFF HERE
			//
			inbuf[nbytes] = '\0';
			s->receiveHandle(inbuf, nbytes, atoi(strrchr(sender_address, '.')+1));
			//printf("Got > %s\n", inbuf);
		}
	}
	return NULL;
}

void dispatcher(sock* s, byte* out_buffer, uint16_t size){
	static bool hassent = false;
	int nbytes;

	if(!hassent)
	{
		byte msg = IGNORE;
		hassent = true;
		dispatcher(s, &msg, 1);
	}
	
	nbytes = sendto(
		s->sd,
		out_buffer,
		size,
		0,
		(struct sockaddr*) &(s->out_addr),
		sizeof(s->out_addr)
	);
	printf("Sent message %s (%d/%d bytes)\n", out_buffer, size, nbytes);
	if (nbytes < 0) {
		perror("dispatcher sendto");
		return;
	}
}
void multicastDispatcher(byte* out_buffer, uint16_t size){
	dispatcher(&(net.multi_s), out_buffer, size);
}

void unicastDispatcher(byte* out_buffer, uint16_t size, uint8_t target_id){
	setupUnicastClient(&(net.uni_s), target_id);
	dispatcher(&(net.uni_s), out_buffer, size);
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
