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


struct decision{
    int state;
    int Nnodes;
    int id;
    int val;
    int PromId;
    int PromVal;
};


struct message{
    int type;   //message type
    int id;   //parameter 1
    int val;   //parameter 2
};

struct node{
    //vetor de decisoes
    struct decision dec_vector[MAX_DECISION];
    int role;

};

//lider -> 1 of os lider 0 otherwise
struct decision innit_decision(int role);
struct node innit_node(int role);

struct decision update_decision_state(struct message msg,struct decision dec);


void print_decision(struct decision dec);
void print_node(struct node n);



#endif