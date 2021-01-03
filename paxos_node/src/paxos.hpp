#ifndef PAXOS_h
#define PAXOS_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//SLEEP TIME
#define SLEEP_TIME 5

//message types , transition types
#define PREPARE_MSG 0
#define PROMISE_MSG 1
#define ACCEPT_MSG  2
#define ACK_MSG     3
#define CLIENT_MSG	255
#define TIMEOUT_MSG 4
#define NULL_MSG -1


//states of the the decisions
#define INNIT_ACCEPTOR  0
#define WAITING_PREPARE 3
#define WAITING_ACEPT   5
#define DECISION_RDY    8 

#define INNIT_PROPOSER  1
#define WAITING_PROMISE 6
#define END_PHASE_1 13

//role
#define ACEPTOR 0
#define PROPOSER 1

//setting
#define MAX_DECISION 1
#define NUMBER_NODES 5


// Identify client to leader messages
#define LEADER_ONLY 1
// Identify messages to ignore (TEMPORARY HACK)
#define IGNORE 3



// multicast identificator
#define MULTICAST -1


struct paxos_state{
    int decisionNumber;
    int state;
    int promMessageVal;
    int promMessageID;

    int currentMessageID ;
    int currentMessageVal;

    int numReceivedPromises;
    int numNodes;
};

struct new_no{
    struct paxos_state paxosStates[MAX_DECISION];
    
    int lastRunedStateId;
    int windowSize;
    int role;
    int liderId;
    int id;
    int lastPhase1complete;
    int lastPhase1innit ;
};

struct transition{
    int decisionNumber;
    int name;
    int messageId;
    int messageVal;
    int originNodeId;
    int dstNodeId;
    int promMessageId;
    int promMessageVal;
};




struct paxos_state update_decision_state_new(struct transition tr,struct paxos_state paxst ,struct  new_no n);
struct paxos_state innit_state_new(int role, int decision_number,int num_nodes );
void send_message_paxos_new (struct transition tr);
struct transition create_new_transition(struct paxos_state paxst,int name,int originNode,int dstNode);


void print_transition(struct transition tr);
void print_message_type(int msgtype);
struct new_no innit_node(int role,int lider_id, int id, int window);
void print_state(struct paxos_state pxs);

void Paxos_logic( void *thread_arg);
void change_role_to_leader(struct new_no *n);
void change_role_to_aceptor(struct new_no *n, int liderid);

struct transition receive_message_paxos_new (char *buff);
#endif
