#include "dataNode.h"
#include <iostream>
#include <string>

using namespace std;

/*main program for executing the DataNode process. Terminates if quit is entered*/
int main(int argc, char** argv) {
  string quit;
  if (argc != 2){
    cout << "Usage: ./main master_node_ip_address\n";
    return 1;
  }
  string master_node_ip = argv[1];
  DataNode* node = new DataNode(master_node_ip);
  thread start(&DataNode::listener, node);
  while (1){
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
