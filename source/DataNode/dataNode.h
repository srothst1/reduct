#pragma once

#include "../config.h"
#include "node.h"

#include <stdexcept>
#include <string>
#include <cstdint>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

/*DataNode class for reading, writing data blocks in to the local file
system upon requests of cleint nodes*/
class DataNode {
  public:
    DataNode(string master_node_ip);
    ~DataNode();
    int32_t getlistenfd();
    string getmaster_node_ip();
    void terminate_all();
    void listener();

  private:
    string master_node_ip;
    string ip;
    vector<thread*> thread_list;
    int32_t listenfd;
    mutex mtx;
    bool terminate_process;
    Node* master_node;
    char buffer[MSG_SIZE];

    void handle_client_request(Node* client);
    void dfs_read(Node* client, string block_ID);
    void dfs_write(Node* client, string block_ID);
};
