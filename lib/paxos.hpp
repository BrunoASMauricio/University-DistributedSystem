#ifndef PAXOS_h
#define PAXOS_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#include "../paxos_node/src/network.hpp"

#include "../paxos_node/src/testing.hpp"


//SLEEP TIME
#define SLEEP_TIME 5

//message types , transition types
#define PREPARE_MSG 1
#define PROMISE_MSG 2
#define ACCEPT_MSG  3
#define ACK_MSG     4
#define CLIENT_MSG	255
#define TIMEOUT_MSG 5
#define NULL_MSG -1
#define ALREADY_AN_ACEPTED_VALUE 6

#define HAS_SOMEONE_DECIDED 7
#define WHERE_IS_MY_PREPARE 8


//states of the the decisions
#define INNIT_ACCEPTOR  1
#define WAITING_PREPARE 2
#define WAITING_ACEPT   3
#define DECISION_RDY    4 

#define INNIT_PROPOSER  5
#define WAITING_PROMISE 6
#define END_PHASE_1 7

//role
#define ACEPTOR 1
#define PROPOSER 2

//setting
#define MAX_DECISION 40


// Identify client to leader messages
#define LEADER_ONLY 1
// Identify messages to ignore (TEMPORARY HACK)
#define IGNORE 3



// multicast identificator
#define MULTICAST -1

// Window Size
#define WINDOW_SIZE 5
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
    int num_nodes;

    pthread_mutex_t lock; 

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
struct new_no innit_node(int role,int lider_id, int id, int window, int nnumb);
void print_state(struct paxos_state pxs);

void * Paxos_logic( void *thread_arg);
void change_role_to_leader(struct new_no *n);
void change_role_to_aceptor(struct new_no *n, int liderid);

struct transition receive_message_paxos_new (char *buff);
#endif
