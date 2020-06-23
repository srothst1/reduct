
#include "masterNode.h"
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

using namespace std;


//main program for executing MasterNode process
int main(int argc, char** argv) {
  string quit;
  MasterNode* node = new MasterNode();
  thread data_listen(&MasterNode::datanode_listener, node);
  thread client_listen(&MasterNode::client_listener, node);
  while (1){
    cin >> quit;
    if (quit == "quit"){
      node->terminate_all();
      break;
    }
  }
  client_listen.join();
  data_listen.join();
  delete node;
  sleep(1);
  return 0;

}
