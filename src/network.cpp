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
	uint16_t nbytes;
	sock* s;
	byte inbuf[1024];
	socklen_t addrlen;

	s = (sock*)_sock;

	while(1){
		nbytes = recvfrom(s->sd,
			inbuf,
			sizeof(inbuf)-1,			// -1 to temporarily accomodate a \0
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
	setupMulticast(&(net.multi_s));
	setupUnicast(&(net.uni_s), id);
	net.id = id;
}
