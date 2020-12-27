#include "../paxos.hpp"


int main(){
    struct new_no lider_no;
    struct new_no n1;



    lider_no =  innit_node(PROPOSER,4,4,1);
    n1       =  innit_node(ACEPTOR, 4,5,1);

    print_state(lider_no.paxosStates[0]);
    print_state(n1.paxosStates[0]);

    /*
    printf("\n--------------------------------\n---Testing lider state machine---\n--------------------------------------------\n");

    struct transition t1 = create_new_transition(lider_no.paxosStates[0],NULL_MSG,-1,-1);
    lider_no.paxosStates[0] = update_decision_state_new(t1,lider_no.paxosStates[0],lider_no);

    print_state(lider_no.paxosStates[0]);
    
    t1 = create_new_transition(lider_no.paxosStates[0],PROMISE_MSG,-1,-1);
    lider_no.paxosStates[0] = update_decision_state_new(t1,lider_no.paxosStates[0],lider_no);

    print_state(lider_no.paxosStates[0]);

    t1 = create_new_transition(lider_no.paxosStates[0],PROMISE_MSG,-1,-1);


    lider_no.paxosStates[0] = update_decision_state_new(t1,lider_no.paxosStates[0],lider_no);

    print_state(lider_no.paxosStates[0]);

    t1 = create_new_transition(lider_no.paxosStates[0],PROMISE_MSG,-1,-1);
    lider_no.paxosStates[0] = update_decision_state_new(t1,lider_no.paxosStates[0],lider_no);
    
    print_state(lider_no.paxosStates[0]);

    t1 = create_new_transition(lider_no.paxosStates[0],PROMISE_MSG,-1,-1);

    t1.promMessageId = 6;
    t1.promMessageVal= 25;
    lider_no.paxosStates[0] = update_decision_state_new(t1,lider_no.paxosStates[0],lider_no);
    
    print_state(lider_no.paxosStates[0]);
    */

    /*
    printf("\n--------------------------------\n---Testing aceptors state machine---\n--------------------------------------------\n");

    struct transition t1 = create_new_transition(n1.paxosStates[0],NULL_MSG,-1,-1);
    n1.paxosStates[0] = update_decision_state_new(t1,n1.paxosStates[0],n1);

    print_state(n1.paxosStates[0]);

    t1 = create_new_transition(n1.paxosStates[0],PREPARE_MSG,23,14);
    n1.paxosStates[0] = update_decision_state_new(t1,n1.paxosStates[0],n1);

    print_state(n1.paxosStates[0]);

    t1 = create_new_transition(n1.paxosStates[0],ACCEPT_MSG,23,14);
    t1.messageId = 2;
    t1.messageVal = 230;

    n1.paxosStates[0] = update_decision_state_new(t1,n1.paxosStates[0],n1);

    print_state(n1.paxosStates[0]);
    */

    printf("\n--------------------------------\n---Testing some real cenarios---\n--------------------------------------------\n");

   
    struct new_no n2;
    struct new_no n3;

    struct transition t1;
    


    lider_no =  innit_node(PROPOSER,4,4,1);
    n1       =  innit_node(ACEPTOR, 4,5,1);
    n2       =  innit_node(ACEPTOR, 4,6,1);
    n3       =  innit_node(ACEPTOR, 4,7,1);

    t1 = create_new_transition(lider_no.paxosStates[0],NULL_MSG,-1,-1);
    lider_no.paxosStates[0] = update_decision_state_new(t1,lider_no.paxosStates[0],lider_no);

    t1 = create_new_transition(n1.paxosStates[0],NULL_MSG,-1,-1);
    n1.paxosStates[0] = update_decision_state_new(t1,n1.paxosStates[0],n1);

    t1 = create_new_transition(n2.paxosStates[0],NULL_MSG,-1,-1);
    n2.paxosStates[0] = update_decision_state_new(t1,n2.paxosStates[0],n2);

    t1 = create_new_transition(n3.paxosStates[0],NULL_MSG,-1,-1);
    n3.paxosStates[0] = update_decision_state_new(t1,n3.paxosStates[0],n3);


    t1 = create_new_transition(lider_no.paxosStates[0],PREPARE_MSG,4,-1);

    n1.paxosStates[0] = update_decision_state_new(t1,n1.paxosStates[0],n1);
    n2.paxosStates[0] = update_decision_state_new(t1,n2.paxosStates[0],n2);
    n3.paxosStates[0] = update_decision_state_new(t1,n3.paxosStates[0],n3);

    t1 = create_new_transition( n1.paxosStates[0],PROMISE_MSG,5,4);
    lider_no.paxosStates[0] = update_decision_state_new(t1,lider_no.paxosStates[0],lider_no);
    lider_no.paxosStates[0] = update_decision_state_new(t1,lider_no.paxosStates[0],lider_no);
    lider_no.paxosStates[0] = update_decision_state_new(t1,lider_no.paxosStates[0],lider_no);



    print_state(lider_no.paxosStates[0]);
    //irs working i believe

}

