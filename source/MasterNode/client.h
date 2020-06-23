#pragma once

#include "secureMap.h"
#include "fileDirectory.h"
#include "node.h"
#include "../config.h"

#include <stdexcept>
#include <string>
#include <cstdint>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

//class for handler thread of each client. Handles requests of a client node
class Client {
  public:
    Client(Node* client_node, FileDirectory* filedirectory,SecureMap<string, int32_t>*blockMAP,
      SecureMap<string,string>*groupMAP, vector<Node*>* data_nodes, mutex* global_mtx);
    ~Client();
    void handle_client_request();
    void terminate_client();

  private:
    char client_buffer[MSG_SIZE];
    char tag;
    int32_t size;
    Node* client_node;
    FileDirectory* filedirectory;
    SecureMap<string, int32_t>*blockMAP;
    SecureMap<string,string>*groupMAP;
    vector<Node*>* data_nodes;
    mutex* global_mtx;
    mutex local_mtx;
    bool terminate_thread;
    // TODO for ls operation
    //vector<string> ls_vector;
    vector<string> parse_directory();
    void dfs_open();
		void dfs_close();
    void dfs_write();
    void dfs_read();
    void dfs_complete();
    //TODO: rest of operations
    void dfs_mkdir();
    void dfs_ls();
    void dfs_check_type();

};

#include "client-inl.h"
