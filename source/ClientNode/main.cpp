
#include "clientNode.h"
#include <iostream>
#include <string>

using namespace std;

//main program for executing  ClientNode process
int main(int argc, char** argv) {
  string quit;
  if (argc != 2){
    cout << "Usage: ./main master_node_ip_address\n";
    return 1;
  }
  string master_node_ip = argv[1];
  ClientNode* node = new ClientNode(master_node_ip);
  thread start(&ClientNode::listener, node);
  while (1){
    //if quit is typed, the process terminates
    cin >> quit;
    if (quit == "quit"){
      node->terminate_all();
      break;
    }
  }
  start.join();
  delete node;
  return 0;

}
