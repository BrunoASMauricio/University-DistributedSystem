
#include "paxos.h"

int main(){
  struct node lider = innit_node(PROPOSER);
  struct node ac1   = innit_node(ACEPTOR);


  struct message msg;
  msg.type  = PREPARE_MSG;   //message type
  msg.id    =  1;  //parameter 1
  msg.val   =  2;


     lider.dec_vector[0] = update_decision_state( msg,lider.dec_vector[0]);
     msg.type = PROMISE_MSG;
     msg.id  = 2;
     msg.val = 2;
     print_node(lider);
  
     lider.dec_vector[0] = update_decision_state( msg,lider.dec_vector[0]);
     print_node(lider);

     msg.type = PROMISE_MSG;
     msg.id  = 7;
     msg.val = 1;
      
     lider.dec_vector[0] = update_decision_state( msg,lider.dec_vector[0]);
     print_node(lider);

     msg.type = PROMISE_MSG;
     msg.id  = 5;
     msg.val = 3;
      
     lider.dec_vector[0] = update_decision_state( msg,lider.dec_vector[0]);
     print_node(lider);
  

  ac1.dec_vector[0] = update_decision_state( msg,ac1.dec_vector[0]);
  print_node(ac1);

  msg.type  = PREPARE_MSG;   //message type
  msg.id    =  10;  //parameter 1
  msg.val   =  3;

  ac1.dec_vector[0] = update_decision_state( msg,ac1.dec_vector[0]);
  print_node(ac1);

  msg.type  = ACCEPT_MSG;   //message type
  msg.id    =  17;  //parameter 1
  msg.val   =  8;

  ac1.dec_vector[0] = update_decision_state( msg,ac1.dec_vector[0]);
  print_node(ac1);
  


}