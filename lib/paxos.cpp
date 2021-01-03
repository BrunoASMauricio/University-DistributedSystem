
#include "paxos.hpp"



//lider -> 1 of os lider 0 otherwise

 int time_out_vec[MAX_DECISION] = {0};


struct paxos_state innit_state_new(int role, int decision_number,int num_nodes ){
    struct  paxos_state pxst;
    if(role == PROPOSER){
             pxst.decisionNumber       = decision_number;
             pxst.state                = INNIT_PROPOSER;
             pxst.numNodes             = num_nodes;
             pxst.numReceivedPromises  =  0;
            
             pxst.currentMessageID     =  1;
             pxst.currentMessageVal    = -1;
             pxst.promMessageID        = -1;
             pxst.promMessageVal       = -1;
    }
    if(role == ACEPTOR){
             pxst.decisionNumber       = decision_number;
             pxst.state                = INNIT_ACCEPTOR;
             pxst.numNodes             = num_nodes;
             pxst.numReceivedPromises  =  0;
           
             pxst.currentMessageID     = -1;
             pxst.currentMessageVal    = -1;
             pxst.promMessageID        = -1;
             pxst.promMessageVal       = -1;
    }
   
    return pxst;

}




void send_message_paxos_new (struct transition tr){
   
    if(tr.dstNodeId == MULTICAST){
        printf("MULTICAST was sent");
        print_transition(tr);
  
        multicastDispatcher((byte *)&tr, sizeof(tr));
    } 
    else{
       
        printf("Unicast was sent");
        print_transition(tr);
  
        unicastDispatcher((byte *)&tr, sizeof(tr),  tr.dstNodeId);
    }
}

struct transition receive_message_paxos_new (char *buff){
    struct transition t1;
    struct transition *thelper =  (struct transition *)buff;
    t1.decisionNumber = thelper->decisionNumber ;
    t1.name           = thelper->name ;
    t1.messageId      = thelper->messageId;
    t1.messageVal     = thelper->messageVal;
    t1.originNodeId   = thelper->originNodeId ;
    t1.dstNodeId      = thelper->dstNodeId ;
    t1.promMessageId  = thelper->promMessageId ;
    t1.promMessageVal = thelper->promMessageVal;
    return t1;

}
struct transition create_new_transition(struct paxos_state paxst,int name,int originNode,int dstNode)
{
    struct transition t1;
    t1.decisionNumber = paxst.decisionNumber;
    t1.name           = name;
    t1.messageId      = paxst.currentMessageID;
    t1.messageVal     = paxst.currentMessageVal;
    t1.originNodeId   = originNode; 
    t1.dstNodeId      = dstNode;
    t1.promMessageId  = paxst.promMessageID;
    t1.promMessageVal = paxst.promMessageVal;
    return t1;
};


struct paxos_state update_decision_state_new(struct transition tr,struct paxos_state paxst ,struct  new_no n){
    struct transition tprepare ;
    struct transition tpromise;
    struct transition taccept;
    
    
    switch (paxst.state)
    {
        case INNIT_ACCEPTOR: {
            paxst.state   =  WAITING_PREPARE;
            break;
        }
        case INNIT_PROPOSER:{
            usleep(SLEEP_TIME * 1000 *10); //for know this is cheating
            paxst.state   =  WAITING_PROMISE;
            tprepare = create_new_transition(paxst,PREPARE_MSG,n.id, MULTICAST);
            send_message_paxos_new (tprepare );
           
            break;
        }

        case  WAITING_PREPARE:{
            if(tr.name == PREPARE_MSG){
                paxst.state   = WAITING_ACEPT;
                paxst.promMessageID  = tr.promMessageId;
                paxst.promMessageVal =  tr.promMessageVal;
                tpromise = create_new_transition(paxst,PROMISE_MSG,n.id, n.liderId);
                send_message_paxos_new(tpromise);
            }
            break;
        }

        case  WAITING_PROMISE:{
            if(tr.name == TIMEOUT_MSG){
                paxst.currentMessageID    +=1;
                paxst.numReceivedPromises = 0;
                paxst.promMessageID  = -1;
                paxst.promMessageVal = -1;
                tprepare = create_new_transition(paxst,PREPARE_MSG,n.id, MULTICAST);
                send_message_paxos_new (tprepare );                
            }
            //falta timeouts para s enao receber promises aumentar o n
            if(tr.name == PROMISE_MSG){ // quer dizer que um no enviou promise aumentar o contador da decisão
                paxst.numReceivedPromises +=1;
                if(tr.promMessageId > paxst.promMessageID){ //se tiver recebido uma mensagem em que o id proposto é maior que os outros que recebeu vai envialo como accept
                    paxst.promMessageID  = tr.promMessageId;
                    paxst.promMessageVal = tr.promMessageVal;
                }
                if(paxst.numReceivedPromises*2 > paxst.numNodes -1){ //alcaçouse a maioria o estado vai transitar
                    paxst.state = END_PHASE_1;
                    time_out_vec[tr.decisionNumber] = 0; //reset timer
                    
                    if(paxst.promMessageID!= -1) {//got an answer different form what it had so it accpets it
                        paxst.state = DECISION_RDY;
                        paxst.currentMessageVal = paxst.promMessageVal;

                        taccept = create_new_transition(paxst,ACCEPT_MSG,n.id, MULTICAST);
                        send_message_paxos_new (taccept ); 
                        break;         
                       
                    }
                } 
            }

            break;
        }

        case  WAITING_ACEPT :{
            if(tr.name == PREPARE_MSG){
                if(tr.messageId > paxst.promMessageID){ //responds
                    paxst.promMessageID  = tr.messageId;
                    paxst.promMessageVal = tr.messageVal;
                    paxst.state = WAITING_ACEPT; 
                    struct transition tpromise = create_new_transition(paxst,PROMISE_MSG,n.id, n.liderId);
                    send_message_paxos_new(tpromise);  
                   }
                else{ //ignores 

                } 

            }
            if(tr.name == ACCEPT_MSG){
                paxst.state = DECISION_RDY;
                paxst.currentMessageID = tr.messageId;
                paxst.currentMessageVal= tr.messageVal;
          
            }
           
            break;
        }

        case END_PHASE_1:{
                if(tr.name == PROMISE_MSG){ // quer dizer que um no enviou promise aumentar o contador da decisão
                        if(tr.promMessageId > paxst.promMessageID){ //se tiver recebido uma mensagem em que o id proposto é maior que os outros que recebeu vai envialo como accept
                                paxst.promMessageID  = tr.promMessageId;
                                paxst.promMessageVal = tr.promMessageVal;
                        }
                        if(paxst.promMessageID!= -1) {//got an answer different form what it had so it accpets it
                            paxst.state = DECISION_RDY;
                            paxst.currentMessageVal = paxst.promMessageVal;

                            taccept = create_new_transition(paxst,ACCEPT_MSG,n.id, MULTICAST);
                            send_message_paxos_new (taccept ); 
                            break;         
                        
                        }
                    
                }
            if(tr.name == CLIENT_MSG){
                paxst.state = DECISION_RDY;
                paxst.currentMessageVal = tr.messageVal;
                taccept = create_new_transition(paxst,ACCEPT_MSG,n.id, MULTICAST);
                send_message_paxos_new (taccept ); 

            }
            break;
        }

    }
    return paxst;

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

void print_state_name(int name){
    switch (name)
    {
        case INNIT_ACCEPTOR:{
            printf("INNIT_ACCEPTOR");
            break;
        }
        
        case WAITING_PREPARE:{
            printf("WAITING_PREPARE");
            break;
        }
        case WAITING_ACEPT:{
            printf("WAITING_ACEPT");
            break;
        }
        case DECISION_RDY : {
            printf("DECISION_RDY ");
            break;
        }
        case INNIT_PROPOSER:{
            printf("INNIT_PROPOSER");
            break;
        }
        
        case WAITING_PROMISE:{
            printf("WAITING_PROMISE");
            break;
        }
        
        case END_PHASE_1:{
            printf("END_PHASE_1");
            break;
        }
    }
}



void print_transition(struct transition tr){
     printf("\n");
     print_message_type(tr.name);

     printf("----------------decNum: %d\n", tr.decisionNumber);
     printf("----------------messID: %d\n", tr.messageId);
     printf("----------------messVal: %d\n", tr.messageVal);
     printf("----------------OriginId: %d\n", tr.originNodeId);
     printf("----------------DstId: %d\n", tr.dstNodeId);
     printf("----------------PromMsgID: %d\n", tr.promMessageId);
     printf("----------------PromMsgVal: %d\n", tr.promMessageVal);

     printf("\n");
}

void print_state(struct paxos_state pxs){
     printf("\n");
     print_state_name(pxs.state);

    printf("\n----decisionNumber: %d\n", pxs.decisionNumber);
    printf("----promMessageVal: %d\n", pxs.promMessageVal);
    printf("----promMessageID: %d\n", pxs.promMessageID);


    printf("----currentMessageID : %d\n", pxs.currentMessageID );
    printf("----currentMessageVal: %d\n", pxs.currentMessageVal);

    printf("----numReceivedPromises: %d\n", pxs.numReceivedPromises);
    printf("----numNodes: %d\n", pxs.numNodes);

    printf("\n");    
}

void print_node(struct new_no n){
    printf("Node role %d\n", n.role);
    printf("Last Phase 1 complete %d\n", n.lastPhase1complete  );
    printf("Last Phase 1 innit %d\n",  n.lastPhase1innit );
    printf("Last Phase Runned State id %d\n",n.lastRunedStateId );
    printf("LiderId %d\n", n.liderId);
    printf("windowSize %d\n", n.windowSize);

    for(int i=0;i< MAX_DECISION;i++){
     
       print_state(n.paxosStates[i]);
    }
}

struct new_no innit_node(int role,int lider_id, int id, int window, int nnode){
    struct new_no n;
    n.windowSize = window;
    n.lastRunedStateId =-1;
    n.lastPhase1innit = -1;
    n.lastPhase1complete = -1;
    n.role = role;
    n.liderId = lider_id;
    n.id = id;
    n.num_nodes = nnode;

    pthread_mutex_init(&(n.lock), NULL);
    return n;
}


void change_role_to_leader(struct new_no *n){
    n->role    = PROPOSER ;
    n->liderId = n->id ;
    struct transition thelper; 
    //repeats phase 1 pf all messages it does not know the answer to
    for (int i=n ->lastRunedStateId+1;i< n->lastPhase1complete ;i++){
        if(n->paxosStates[i].state != DECISION_RDY ){
                n->paxosStates[i] = innit_state_new(PROPOSER, i, n->num_nodes );
                thelper = create_new_transition( n->paxosStates[i],NULL_MSG,-1,-1);
                n->paxosStates[i] = update_decision_state_new(thelper,n->paxosStates[i],*n);
        }
    }
   
}

void change_role_to_aceptor(struct new_no *n, int liderid){
    n->role = ACEPTOR ;
    n->liderId = liderid ;
    struct transition thelper; 
    // changes its role to aceptor and for the states it does not have a decision it changes to An acepto statemachine
    for (int i=n ->lastRunedStateId+1;i< n->lastPhase1complete ;i++){
        if(n->paxosStates[i].state != DECISION_RDY ){
                n->paxosStates[i] = innit_state_new(ACEPTOR, i,n->num_nodes );
                thelper = create_new_transition( n->paxosStates[i],NULL_MSG,-1,-1);
                n->paxosStates[i] = update_decision_state_new(thelper,n->paxosStates[i],*n);
        }
    }
   
}



void * Paxos_logic( void *thread_arg)  
{
    struct new_no *n = (struct new_no *)thread_arg;
    struct transition thelper; 
  
    print_node(*n);

    printf("Stated paxos logic\n");
   

    while(1){

        pthread_mutex_lock(&(n->lock)); 
        //printf("running cycle\n");
        if(n->paxosStates[n->lastRunedStateId+1].state == DECISION_RDY){
            printf("Decision was made\n");
            printf("Decision %d can be runned val %d:\n",n->lastRunedStateId+1,n->paxosStates[n->lastRunedStateId+1].currentMessageVal);
            n->lastRunedStateId++;
            if (n->lastRunedStateId == MAX_DECISION -1){
                break;
            }
        }

        if(n->lastPhase1innit == n->lastRunedStateId){
            printf("inniting new window\n");
          
            for (int i=n->lastRunedStateId+1;i<n->lastRunedStateId+1+n->windowSize;i++){
                printf("inniting decision %d\n",i);
                n->paxosStates[i] = innit_state_new(n->role, i, n->num_nodes );           
                thelper = create_new_transition( n->paxosStates[i],NULL_MSG,-1,-1);
                n->paxosStates[i] = update_decision_state_new(thelper,n->paxosStates[i],*n);
            }
            n->lastPhase1innit = n->lastRunedStateId + n->windowSize;

        }
        //updates timers for timeout and activates it in case of timeout, timeouts are counters easier
        for (int i=n->lastRunedStateId+1;i<n->lastRunedStateId+1+n->windowSize;i++){
            if( n->paxosStates[i].state == WAITING_PROMISE){
                time_out_vec[i] += 1;
                if(time_out_vec[i] == 1000){
                    printf("Timing out decision %d",i);
                    time_out_vec[i] = 0;
                    thelper = create_new_transition( n->paxosStates[i],TIMEOUT_MSG,-1,-1);
                    n->paxosStates[i] = update_decision_state_new(thelper,n->paxosStates[i],*n);
              
                    
                } 
            }
        }

        pthread_mutex_unlock(&(n->lock)); 
        
        
       
        usleep(SLEEP_TIME*1000);
    }

    printf("All decisions were made\n");
};

