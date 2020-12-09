#ifndef PAXOS_h
#define PAXOS_h

#include <stdio.h>
#include <stdlib.h>

#define NUMBER_NODES 5

//message types
#define PREPARE_MSG 0
#define PROMISE_MSG 1
#define ACCEPT_MSG  2
#define ACK_MSG     3

#define TIMEOUT_MSG 4

//states of the the decisions
#define INNIT_ACCEPTOR  0
#define INNIT_PROPOSER  1
#define WAITING_PREPARE 3
#define WAITING_ACEPT   5
#define WAITING_PROMISE 6
#define DECISION_RDY    8 
//setting
#define MAX_DECISION 1
//role
#define ACEPTOR 0
#define PROPOSER 1

#define MULTICAST -1


struct decision{
    int state;
    int Nnodes;
    int id;
    int val;
    int PromId;
    int PromVal;

    int decision_number;
};


struct message{
    int decision_number;
    int node_origin;

    int type;   //message type
    int id;   //parameter 1
    int val;   //parameter 2
};

struct node{
    //vetor de decisoes
    struct decision dec_vector[MAX_DECISION];
    int role;
    int lastprocessed_dec;
    int lider_id;
    int id;


};


void client_set_value ( struct decision *dec,int val);

void send_message_paxos (int type, struct decision dec,int to_who, int bywho);
//lider -> 1 of os lider 0 otherwise
struct decision innit_decision(int role,int decision_number);
struct node innit_node(int role, int lider_id, int id);

struct decision update_decision_state(struct message msg,struct decision dec,struct  node n);

void print_decision(struct decision dec);
void print_node(struct node n);



#endif