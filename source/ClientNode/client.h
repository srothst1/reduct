#pragma once

#include "fileDescriptor.h"
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



/* Class for client threads, each of which handles requests from a client program.
Receeives reqeusts from the client program and handle them appropriately.
*/
class Client {
  public:
    Client(int32_t client_num, int32_t sockfd, Node* master_node,
      mutex* global_mtx1, mutex* global_mtx2, bool* terminate_process);
    ~Client();
    int32_t getclient_num();
    int32_t getsockfd();
    void handle_client_request();
    void terminate_client();

  private:
    int32_t client_num;
    int32_t sockfd;
    vector<FileDescriptor*> file_table;
    char client_buffer[MSG_SIZE];
    Node* master_node;
    char tag;
    int32_t size;
    mutex* global_mtx1;
    mutex* global_mtx2;
    mutex local_mtx;
    bool* terminate_process;
    bool terminate_thread;
    // TODO for ls operation
    //vector<string> ls_vector;

    int32_t local_send();
    int32_t local_recv();
    void terminate_all();
    void dfs_open();
		void dfs_close(int32_t file_id);
    void dfs_write(int32_t file_id, int32_t block_size, string block_id);
    void dfs_read();

    //TODO: rest of operations
    void dfs_mkdir();
    void dfs_ls();
    void dfs_check_type();


};
