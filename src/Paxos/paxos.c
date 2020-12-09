#include <stdio.h>
#include <stdlib.h>
#include "paxos.h"

//lider -> 1 of os lider 0 otherwise
struct decision innit_decision(int role){
    struct  decision dec;
    if(role == PROPOSER){
             dec.state  = INNIT_PROPOSER;
             dec.Nnodes = 0;
             dec.id     = 1;
             dec.val    = -1;
             dec.PromId = -1;
             dec.PromVal= -1;
    }
    if(role == ACEPTOR){
             dec.state  = INNIT_ACCEPTOR;
             dec.Nnodes = 0;
             dec.id     = -1;
             dec.val    = -1;
             dec.PromId = -1;
             dec.PromVal= -1;
    }
    return dec;
    
}

struct decision update_decision_state(struct message msg,struct decision dec){
    switch ( dec.state)
    {
        case INNIT_ACCEPTOR:
            dec.state   =  WAITING_PREPARE;
            dec.id      = -1;
            dec.val     = -1;
            dec.PromVal = -1;
            dec.PromId  = -1;
            dec.Nnodes  = 0;
            break;

        case INNIT_PROPOSER:
            dec.state   =  WAITING_PROMISE;
            dec.id      =  msg.id;
            dec.val     = -1;
            dec.PromVal = -1;
            dec.PromId  = -1;
            dec.Nnodes  =  0;
            printf("Prepared message was sent");
            break;

        case  WAITING_PREPARE:
            if(msg.type == PREPARE_MSG){
                dec.state   = WAITING_ACEPT;
                dec.PromId  = msg.id;
                dec.PromVal = msg.val;
                printf("Promised message was sent");
            }
            break;


        case  WAITING_PROMISE:
            //falta timeouts para s enao receber promises aumentar o n
            if(msg.type == PROMISE_MSG){ // quer dizer que um no enviou promise aumentar o contador da decisão
                dec.Nnodes +=1;
                if(msg.id > dec.PromId){ //se tiver recebido uma mensagem em que o id proposto é maior que os outros que recebeu vai envialo como accept
                    dec.PromId  = msg.id;
                    dec.PromVal = msg.val;
                }
                if(dec.Nnodes*2 > NUMBER_NODES){ //alcaçouse a maioria o estado vai transitar
                    dec.state = DECISION_RDY;
                    printf("Accept message was sent");

                } 
            }
            break;

        case  WAITING_ACEPT :
            if(msg.type == PREPARE_MSG){
                if(msg.id > dec.PromId){ //responds
                    dec.PromId  = msg.id;
                    dec.PromVal = msg.val;
                    dec.state = WAITING_ACEPT;    
                    printf("Promised message was sent");    }
                else{ //ignores 

                } 

            }
            if(msg.type == ACCEPT_MSG){
                dec.id  = msg.id;
                dec.val = msg.val;
                dec.state = DECISION_RDY;
            }
           
            break;

        
        default:
            break;
    }
    return dec;

}



void print_decision(struct decision dec){
    printf("----state: %d\n", dec.state);
    printf("----Nnodes: %d\n", dec.Nnodes);
    printf("----id: %d\n", dec.id);
    printf("----val: %d\n", dec.val);
    printf("----Promid %d\n",dec.PromId);
    printf("----Promval %d\n", dec.PromVal);
}

void print_node(struct node n){
    printf("Node role %d\n", n.role);
    for(int i=0;i<MAX_DECISION;i++){
       printf("--Decision number %d\n",i);
       print_decision(n.dec_vector[i]);
    }
}

struct node innit_node(int role){
    struct node n;
    n.role = role;
    for(int i=0;i<MAX_DECISION;i++){
        n.dec_vector[i] = innit_decision(role);
    }
    return n;
}
