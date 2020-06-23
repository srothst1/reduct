#pragma once

#include "../config.h"
#include "secureMap.h"
#include "fileDirectory.h"
#include "client.h"

#include <stdexcept>
#include <string>
#include <cstdint>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

/*Class for executing the MasterNode process. Stores all file system metadata. Each
client object handle client node requests and access/modify the metadata accordingly.
*/
class MasterNode {
  public:
    MasterNode();
    ~MasterNode();
    int32_t getlistenfd1();
    int32_t getlistenfd2();
    void terminate_all();
    void client_listener();
    void datanode_listener();

  private:
    vector<Client*> client_table;
    vector<Node*> data_nodes;
    vector<thread*> client_thread_list;
    FileDirectory* filedirectory;
    SecureMap<string, int32_t>*blockMAP;
    SecureMap<string, string>*groupMAP;
    int32_t listenfd1;
    int32_t listenfd2;
    mutex global_mtx;
    bool terminate_process;
    char buffer[MSG_SIZE];

};

#include "masterNode-inl.h"
