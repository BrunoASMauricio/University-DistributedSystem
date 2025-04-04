#ifndef PAXOS_h
#define PAXOS_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/time.h>


#include <time.h> 


#include "../paxos_node/src/network.hpp"

#include "../paxos_node/src/testing.hpp"



//SLEEP TIME
#define SLEEP_TIME 5

//message types , transition types
#define PREPARE_MSG 5
#define PROMISE_MSG 6
#define ACCEPT_MSG  7
#define ACK_MSG     8
#define CLIENT_MSG	255
#define TIMEOUT_MSG 9
#define NULL_MSG 10
#define ALREADY_AN_ACEPTED_VALUE 11
#define ACCEPTED_MSG 999

#define HAS_SOMEONE_DECIDED 12
#define WHERE_IS_MY_PREPARE 13


//states of the the decisions
#define INNIT_ACCEPTOR  14
#define WAITING_PREPARE 15
#define WAITING_ACEPT   16
#define DECISION_RDY    17

#define INNIT_PROPOSER  18
#define WAITING_PROMISE 19
#define END_PHASE_1 20

//role
#define ACEPTOR 1
#define PROPOSER 2

#define CLIENT_ID 777

//setting
#define MAX_DECISION 6


// Identify client to leader messages
#define LEADER_ONLY 34
// Identify messages to ignore (TEMPORARY HACK)
#define IGNORE 35

//#define TEST

// multicast identificator
#define MULTICAST 45

// Window Size
#define WINDOW_SIZE 2
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
    
    int n_recv_messages;
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
