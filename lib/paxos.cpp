
#include "paxos.hpp"


#ifdef TEST
int client_val[MAX_DECISION];
#endif

int mes_number;
int fin;


struct timeval elapsed_time, innit_time;
 

//lider -> 1 of os lider 0 otherwise

 int time_out_vec[MAX_DECISION] = {0};
 int results_nt[MAX_DECISION] = {0};
 int results[MAX_DECISION] = {0};


 double results_time[MAX_DECISION] = {0};

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
        //printf("MULTICAST was sent");
        print_transition(tr);
  
        multicastDispatcher((byte *)&tr, sizeof(tr), PAXOS);
    } 
    else{
       
        //printf("Unicast was sent");
        print_transition(tr);
  
        unicastDispatcher((byte *)&tr, sizeof(tr),  tr.dstNodeId, PAXOS);
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
    struct transition talr_acept;

    if(tr.name != TIMEOUT_MSG){
        mes_number +=1;
    }
    
    /*
    if(paxst.state == DECISION_RDY && tr.name != TIMEOUT_MSG){
          talr_acept = create_new_transition(paxst,ALREADY_AN_ACEPTED_VALUE,n.id, tr.originNodeId);
          send_message_paxos_new(talr_acept);   
          return paxst;  
    }*/
    if(tr.name == PREPARE_MSG && paxst.state == DECISION_RDY){
          talr_acept = create_new_transition(paxst,ALREADY_AN_ACEPTED_VALUE,n.id, tr.originNodeId);
          send_message_paxos_new(talr_acept);         
    }

    if(tr.name == ALREADY_AN_ACEPTED_VALUE){
        paxst.state = DECISION_RDY;
        paxst.currentMessageID = tr.messageId;
        paxst.currentMessageVal= tr.messageVal;
        return paxst;
    }

    if(tr.name == HAS_SOMEONE_DECIDED ){
        if( paxst.state == DECISION_RDY){
              talr_acept = create_new_transition(paxst,ALREADY_AN_ACEPTED_VALUE,n.id, tr.originNodeId);
              send_message_paxos_new(talr_acept);            
        }
        return paxst;

    }

     if(tr.name ==  WHERE_IS_MY_PREPARE ){
        if( paxst.state == DECISION_RDY){
              talr_acept = create_new_transition(paxst,ALREADY_AN_ACEPTED_VALUE,n.id, tr.originNodeId);
              send_message_paxos_new(talr_acept);            
        }else{
            tprepare = create_new_transition(paxst,PREPARE_MSG,n.id, MULTICAST);
            send_message_paxos_new (tprepare ); 
        }
         return paxst;


    }

    
    switch (paxst.state)
    {
        case INNIT_ACCEPTOR: {
            time_out_vec[tr.decisionNumber] = 0; 
            paxst.state   =  WAITING_PREPARE;
            break;
        }
        case INNIT_PROPOSER:{
            time_out_vec[tr.decisionNumber] = 0; 
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
                time_out_vec[tr.decisionNumber] = 0;   
            }

            if(tr.name == TIMEOUT_MSG){
                tprepare = create_new_transition(paxst,WHERE_IS_MY_PREPARE,n.id, n.liderId);
                send_message_paxos_new (tprepare );   
                time_out_vec[tr.decisionNumber] = 0;                
            }

            break;
        }

        case  WAITING_PROMISE:{
            if(tr.name == TIMEOUT_MSG){
                paxst.currentMessageID    +=1;
                paxst.numReceivedPromises = 0;
                paxst.promMessageID  = -1;
                paxst.promMessageVal = -1;
                time_out_vec[tr.decisionNumber] = 0;
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
                if(paxst.numReceivedPromises*2 > paxst.numNodes -2){ //alcaçouse a maioria o estado vai transitar
                    paxst.state = END_PHASE_1;
                    time_out_vec[tr.decisionNumber] = 0; //reset timer
                    
                    if(paxst.promMessageVal!= -1) {//got an answer different form what it had so it accpets it
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
            // a timer here in case of failing in receiving the acept it can inquire
             if(tr.name == TIMEOUT_MSG){ 
                tprepare = create_new_transition(paxst,HAS_SOMEONE_DECIDED,n.id, MULTICAST);
                send_message_paxos_new (tprepare );  
                time_out_vec[tr.decisionNumber] = 0;              
            }

            if(tr.name == PREPARE_MSG){
                if(tr.messageId > paxst.promMessageID){ //responds
                    paxst.promMessageID  = tr.messageId;
                    paxst.promMessageVal = tr.messageVal;
                    paxst.state = WAITING_ACEPT; 
                    struct transition tpromise = create_new_transition(paxst,PROMISE_MSG,n.id, n.liderId);
                    send_message_paxos_new(tpromise); 
                    time_out_vec[tr.decisionNumber] = 0;      
                   }
                else{ //ignores 

                } 

            }
            if(tr.name == ACCEPT_MSG){
                paxst.state = DECISION_RDY;
                paxst.currentMessageID = tr.messageId;
                paxst.currentMessageVal= tr.messageVal;
                time_out_vec[tr.decisionNumber] = 0;     
            }
           
            break;
        }

        case END_PHASE_1:{
                if(tr.name == PROMISE_MSG){ // quer dizer que um no enviou promise aumentar o contador da decisão
                        if(tr.promMessageId > paxst.promMessageID){ //se tiver recebido uma mensagem em que o id proposto é maior que os outros que recebeu vai envialo como accept
                                paxst.promMessageID  = tr.promMessageId;
                                paxst.promMessageVal = tr.promMessageVal;
                        }
                        if(paxst.promMessageVal!= -1) {//got an answer different form what it had so it accpets it
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
        case DECISION_RDY:{
           
             
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
    
    case ALREADY_AN_ACEPTED_VALUE:
        printf("ALREADY_AN_ACEPTED_VALUE_MSG");
        break;
    
     case HAS_SOMEONE_DECIDED :
        printf("HAS_SOMEONE_DECIDED _MSG");
        break;
    case WHERE_IS_MY_PREPARE :
        printf("Where is my prepare");
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
 
     printf(" ");
     print_message_type(tr.name);

    /*
     printf("----------------decNum: %d\n", tr.decisionNumber);
     printf("----------------messID: %d\n", tr.messageId);
     printf("----------------messVal: %d\n", tr.messageVal);
     printf("----------------OriginId: %d\n", tr.originNodeId);
     printf("----------------DstId: %d\n", tr.dstNodeId);
     printf("----------------PromMsgID: %d\n", tr.promMessageId);
     printf("----------------PromMsgVal: %d\n", tr.promMessageVal);
     */
     printf("\n");
}

void print_state(struct paxos_state pxs){
     printf("\n");
     print_state_name(pxs.state);
  
    printf(" dec numb: %d\n", pxs.decisionNumber);
    /*
    printf("----promMessageVal: %d\n", pxs.promMessageVal);
    printf("----promMessageID: %d\n", pxs.promMessageID);


    printf("----currentMessageID : %d\n", pxs.currentMessageID );
    printf("----currentMessageVal: %d\n", pxs.currentMessageVal);

    printf("----numReceivedPromises: %d\n", pxs.numReceivedPromises);
    printf("----numNodes: %d\n", pxs.numNodes);
    */
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

    #ifdef TEST
     
    
    srandom(124*id);

    for(int i= 0;i<MAX_DECISION;i++){
        client_val[i] = random()%1000;
    }
    #endif
    gettimeofday(&innit_time, NULL);
//do stuff

    
    fin = 0;
    mes_number = 0;
    struct new_no n;
    memset(&n,0,sizeof(n));
    
    n.windowSize = window;
    n.lastRunedStateId =-1;
    n.lastPhase1innit = -1;
    n.lastPhase1complete = -1;
    n.role = role;
    n.liderId = lider_id;
    n.id = id;
    n.num_nodes = nnode;
    n.n_recv_messages = 0;
    pthread_mutex_init(&(n.lock), NULL);
    return n;
}


void change_role_to_leader(struct new_no *n){
    printf("CHANGED ROLE TO LEADER MANINHO MEGA FIXE\n");
    printf("CHANGED ROLE TO LEADER MANINHO MEGA FIXE\n");
    printf("CHANGED ROLE TO LEADER MANINHO MEGA FIXE\n");
    n->role    = PROPOSER ;
    n->liderId = n->id ;
    struct transition thelper; 
    //repeats phase 1 pf all messages it does not know the answer to
    printf("last Runed Stae ID     %d\n",n ->lastRunedStateId +1  );
    printf("last phase 1 innited %d\n",n->lastPhase1innit );

   for (int i=n->lastRunedStateId+1;i< n->lastPhase1innit+1 ;i++){
        if(n->paxosStates[i].state != DECISION_RDY ){
                n->paxosStates[i] = innit_state_new(PROPOSER, i, n->num_nodes );
                thelper = create_new_transition( n->paxosStates[i],NULL_MSG,-1,-1);
                n->paxosStates[i] = update_decision_state_new(thelper,n->paxosStates[i],*n);
        }
    }
   
}

void change_role_to_aceptor(struct new_no *n, int liderid){
    printf("CHANGED ROLE TO Aceptor\n");
    printf("CHANGED ROLE TO Aceptor\n");
    printf("CHANGED ROLE TO Aceptor\n");
    n->role = ACEPTOR ;
    n->liderId = liderid ;
    struct transition thelper; 

    printf("LAST RUNNES STATE ID +1 : %d\n",n ->lastRunedStateId+1);
    printf("LAST PHASE 1 INNIT +1 : %d \n",n->lastPhase1innit +1);
    // changes its role to aceptor and for the states it does not have a decision it changes to An acepto statemachine
    for (int i=n ->lastRunedStateId+1;i< n->lastPhase1innit +1   ;i++){
        if(n->paxosStates[i].state != DECISION_RDY ){
                n->paxosStates[i] = innit_state_new(ACEPTOR, i,n->num_nodes );
                thelper = create_new_transition( n->paxosStates[i],NULL_MSG,-1,-1);
                n->paxosStates[i] = update_decision_state_new(thelper,n->paxosStates[i],*n);
        }
    }
   
}


void * Paxos_logic(void *thread_arg)  
{
    static struct new_no *n = (struct new_no *)thread_arg;
    static struct transition thelper; 
  
    
	usleep(SLEEP_TIME*200);


	pthread_mutex_lock(&(n->lock)); 
	//printf("running cycle\n");
    #ifdef TEST
    for (int i=n->lastRunedStateId+1;i<n->lastRunedStateId+1+n->windowSize;i++){
		if( n->paxosStates[i].state == END_PHASE_1){
            	thelper.name = CLIENT_MSG;
				//obv isto vai mudar mas por enquanto fica
				thelper.messageVal = client_val[i];
				thelper.dstNodeId  = MULTICAST;
				thelper.decisionNumber = i;
				n->paxosStates[i] = update_decision_state_new(thelper,n->paxosStates[i],*n);
		}
	}

    #endif

	if(n->paxosStates[n->lastRunedStateId+1].state == DECISION_RDY){
		printf("Decision was made\n");
		printf("Decision %d can be runned val %d:\n",n->lastRunedStateId+1,n->paxosStates[n->lastRunedStateId+1].currentMessageVal);
		results_nt[n->lastRunedStateId+1] = mes_number;
        results[n->lastRunedStateId+1] =n->paxosStates[n->lastRunedStateId+1].currentMessageVal;
        gettimeofday(&elapsed_time, NULL);
        
        double a =(elapsed_time.tv_sec*1.0 - innit_time.tv_sec*1.0)*1.0 + (elapsed_time.tv_usec*1.0 - innit_time.tv_usec*1.0)/1000000;

        results_time[n->lastRunedStateId+1] = a;
        n->lastRunedStateId++;
		if (n->lastRunedStateId == MAX_DECISION -1 && fin==0){
            fin = 1;
            n->lastPhase1innit = MAX_DECISION-1;
            char bfil[200];
            for(int i= 0;i<MAX_DECISION;i++){
               printf("Dec %d vall %d nmess %d\n",i,results[i],results_nt[i]);
               snprintf(bfil, sizeof(bfil), "Dec %d vall %d num_mens %d time %f\n",i,results[i],results_nt[i],results_time[i]);
               writeToFile(bfil, n->id);
            }
            sleep(20);
           

           
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
    int del = 5000;
    #ifdef TEST
    del = 50;
    #endif
	//updates timers for timeout and activates it in case of timeout, timeouts are counters easier
	for (int i=n->lastRunedStateId+1;i<n->lastRunedStateId+1+n->windowSize;i++){
		if( (n->paxosStates[i].state == WAITING_PROMISE) || (n->paxosStates[i].state == WAITING_ACEPT) || (n->paxosStates[i].state == WAITING_PREPARE)){
			time_out_vec[i] += 1;
			if(time_out_vec[i] == del){
				printf("Timing out decision %d",i);
				time_out_vec[i] = 0;
				thelper = create_new_transition( n->paxosStates[i],TIMEOUT_MSG,-1,-1);
				n->paxosStates[i] = update_decision_state_new(thelper,n->paxosStates[i],*n);
		  
				
			} 
		}
	}
    if (n->lastRunedStateId == MAX_DECISION -1){
        n->lastPhase1innit = MAX_DECISION;
    }
	pthread_mutex_unlock(&(n->lock)); 
	
	
   
	return NULL;
};

