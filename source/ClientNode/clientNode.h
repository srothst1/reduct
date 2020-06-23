#pragma once

#include "client.h"
#include "../config.h"

#include <stdexcept>
#include <string>
#include <cstdint>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

/* ClientNode Class for executing an instance of ClientNode process on a client node.
Any client program accesses the distributed file system through ClientNode */
class ClientNode {
  public:
    ClientNode(string master_node_ip);
    ~ClientNode();
    int32_t getlistenfd();
    string getmaster_node_ip();
    void terminate_all();
    void listener();

  private:
    string master_node_ip;
    vector<Client*> client_table;
    vector<thread*> thread_list;
    int32_t listenfd;
    mutex global_mtx1;
    mutex global_mtx2;
    bool terminate_process;
    Node* master_node;
    char buffer[MSG_SIZE];

};
