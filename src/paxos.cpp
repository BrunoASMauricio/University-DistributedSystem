
#include "paxos.hpp"

#include "network.hpp"


//lider -> 1 of os lider 0 otherwise



void client_set_value ( struct decision *dec,int val){
    (*dec).val = val;
    return;
}





struct decision innit_decision(int role, int decision_number){
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
    dec.decision_number = decision_number;

    return dec;
    
}


void print_message_type(int msgtype){
    switch (msgtype)
    {
    case PREPARE_MSG:
        printf("PREPARE_MSG");
        break;
    
    case PROMISE_MSG:
        printf("PROMISE_MSG");
        break;
    case ACCEPT_MSG:
         printf("ACCEPT_MSG");
        break;
    case ACK_MSG: 
         printf("ACK_MSGG");
        break;
    case TIMEOUT_MSG:
        printf("TIMEOUT_MSG");
        break;
    }
}





void send_message_paxos (int type, struct decision dec,int to_who, int bywho)
{
    struct message msg;
    char buff[40];

    msg.decision_number = dec.decision_number;
    msg.node_origin     = bywho;
    msg.type            = type;
    
    if(msg.type == PREPARE_MSG){
        msg.id  = dec.id;
        msg.val = -1;
    }

    if(msg.type == PROMISE_MSG){
        msg.id  = dec.PromId;
        msg.val = dec.PromVal;
    }

    if(msg.type == ACCEPT_MSG){
        msg.id  = dec.id;
        msg.val = dec.val;
    }

    memcpy (buff,&msg,sizeof(msg));

    if(to_who == MULTICAST){
        
	    multicastDispatcher(buff, sizeof(msg));
    }
    else{
        unicastDispatcher(buff, sizeof(msg), bywho);
    }
 

   
    
    printf("-----------------Message sent to %d by: %d ",to_who,bywho);
    printf("msg id: %d msg val: %d dec_number %d ", msg.id,msg.val, msg.decision_number);
    printf("type: ");
    print_message_type(type);
    printf("\n");
};


struct decision update_decision_state(struct message msg,struct decision dec,struct  node n){
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
            //innit a timeout
            send_message_paxos (PREPARE_MSG, dec,MULTICAST,n.id);

            break;

        case  WAITING_PREPARE:
            if(msg.type == PREPARE_MSG){
                dec.state   = WAITING_ACEPT;
                dec.PromId  = msg.id;
                dec.PromVal = msg.val;

                send_message_paxos (PROMISE_MSG, dec,n.lider_id,n.id);
            }
            break;


        case  WAITING_PROMISE:
            if(msg.type == TIMEOUT_MSG){
                dec.id +=1;
                dec.state = WAITING_PROMISE;
                send_message_paxos( PREPARE_MSG,dec,MULTICAST,n.id);
               

            }
            //falta timeouts para s enao receber promises aumentar o n
            if(msg.type == PROMISE_MSG){ // quer dizer que um no enviou promise aumentar o contador da decisão
                dec.Nnodes +=1;
                if(msg.id > dec.PromId){ //se tiver recebido uma mensagem em que o id proposto é maior que os outros que recebeu vai envialo como accept
                    dec.PromId  = msg.id;
                    dec.PromVal = msg.val;
                }
                if(dec.Nnodes*2 > NUMBER_NODES){ //alcaçouse a maioria o estado vai transitar
                    dec.state = DECISION_RDY;
                    if(dec.PromId != -1) {//got an answer different form what it had so it accpets it
                        dec.id = dec.PromId;
                        dec.val= dec.PromVal;

                    }

                    //phase two of paxos (so it can wait before sending an accpt for the client puposal)
                    send_message_paxos( ACCEPT_MSG,dec,MULTICAST,n.id);
                    //printf("Accept message was sent");

                } 
            }
            break;

        case  WAITING_ACEPT :
            if(msg.type == PREPARE_MSG){
                if(msg.id > dec.PromId){ //responds
                    dec.PromId  = msg.id;
                    dec.PromVal = msg.val;
                    dec.state = WAITING_ACEPT; 
                    send_message_paxos( PROMISE_MSG,dec,n.lider_id,n.id);   
                   }
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

struct node innit_node(int role,int lider_id, int id){
    struct node n;
    n.lastprocessed_dec =-1;
    n.role = role;
    n.lider_id = lider_id;
    n.id = id;
    for(int i=0;i<MAX_DECISION;i++){
        n.dec_vector[i] = innit_decision(role,i);
    }
    return n;
}
