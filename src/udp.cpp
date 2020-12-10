#include "udp.hpp"

void startSocket(sock* s){
	if((s->sd = socket(AF_INET, SOCK_DGRAM, 0)) < 0){
		perror("Opening UDP socket error");
		exit(-1);
	}
	printf("UDP socket open\n");
	//socketBlocking(s, true);
}

void socketBlocking(sock* s, bool set_block){
	int opt = fcntl(s->sd, F_GETFL);
    if (opt < 0){
        printf("fcntl(F_SETFL) fail.");
		exit(-1);
    }

	if(set_block){
		//SETBIT(O_NONBLOCK, opt);
		printf("Socket set for non blocking\n");
	}else{
		//CLEARBIT(O_NONBLOCK, opt);
		printf("Socket set for blocking\n");
	}

    if (fcntl(s->sd, F_SETFL, opt) < 0){
        printf("fcntl(F_SETFL) fail.");
		exit(-1);
	}

}

// --------- Multicast ---------
void bindMulticastServer(sock* s){
    u_int yes = 1;
	struct in_addr localInterface;

	// Set port to be reused (all nodes use the same 2 ports)
    if (setsockopt(s->sd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
	   exit(EXIT_FAILURE);
    }
	printf("Socket set up to be reused\n");

	//
    memset(&(s->in_addr), 0, sizeof(s->in_addr));
    s->in_addr.sin_family = AF_INET;
    s->in_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    s->in_addr.sin_port = htons(MULTICAST_PORT);

	// Bind to receive address
    if (bind(s->sd, (struct sockaddr*) &(s->in_addr), sizeof(s->in_addr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
    }

	localInterface.s_addr = inet_addr (s->interface_addr);

	if (setsockopt(s->sd, IPPROTO_IP, IP_MULTICAST_IF, (char *) &localInterface,sizeof (localInterface)) < 0){
		perror ("Setting local interface error");
		close(s->sd);
		exit(EXIT_FAILURE);
	}
	printf ("Setting the local interface...OK\n");

	// use setsockopt() to request that the kernel join a multicast group
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);//inet_addr(s->interface_addr);//
    if (setsockopt(s->sd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, sizeof(mreq)) < 0){
        perror("setsockopt");
		close(s->sd);
		exit(EXIT_FAILURE);
    }
	printf("Request to join multicast group accepted\n");

	// Force only 1 multicast stream
	int mc_all = 0;
	if ((setsockopt(s->sd, IPPROTO_IP, IP_MULTICAST_ALL, (void*) &mc_all, sizeof(mc_all))) < 0) {
		perror("setsockopt() failed");
	}

	char loopch = 1;
	if(setsockopt(s->sd, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopch, sizeof(loopch)) < 0)
	{
		perror("Setting IP_MULTICAST_LOOP error");
		close(s->sd);
		exit(EXIT_FAILURE);
	}
	printf("Enabling the loopback...OK.\n");
}


void startMulticastClient(sock* s){
    u_int yes = 1;
    if (setsockopt(s->sd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
	   exit(EXIT_FAILURE);
    }
    memset(&(s->out_addr), 0, sizeof(s->out_addr));
	s->out_addr.sin_family = AF_INET;
    s->out_addr.sin_addr.s_addr = inet_addr(MULTICAST_GROUP);
    s->out_addr.sin_port = htons(MULTICAST_PORT);
}


// --------- Unicast ---------

void bindUnicastServer(sock* s){
	u_int yes = 1;
    if (setsockopt(s->sd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)) < 0) {
       perror("Reusing ADDR failed");
	   exit(EXIT_FAILURE);
    }
	printf("Socket set up to be reused\n");

    memset(&(s->in_addr), 0, sizeof(s->in_addr));
    s->in_addr.sin_family = AF_INET;
    s->in_addr.sin_addr.s_addr =inet_addr(s->interface_addr);
    s->in_addr.sin_port = htons(UNICAST_PORT);

	// Bind to receive address
    if (bind(s->sd, (struct sockaddr*) &(s->in_addr), sizeof(s->in_addr)) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
    }
}

void setupUnicastClient(sock* s, int target_addr){
	char addr[16] = INTERFACE_BASE_IP;
	addr[strlen(addr)-1] = '0'+target_addr;

    memset(&(s->out_addr), 0, sizeof(s->out_addr));
	s->out_addr.sin_family = AF_INET;
	s->out_addr.sin_addr.s_addr = inet_addr(addr);
	s->out_addr.sin_port = htons(UNICAST_PORT);
}

void setupUnicast(sock* s, int id){
	startSocket(s);
	bindUnicastServer(s);
}


