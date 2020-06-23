#pragma once

#include "../config.h"
#include "fileDescriptor.h"

#include <stdexcept>
#include <string>
#include <cstring>
#include <cstdint>
#include <vector>

using namespace std;

/* Class for FileObject a client program initializes to access the file system. 
*/
class FileObject {
  public:
    FileObject(string ip);
    ~FileObject();
    int32_t getsockfd();
    int32_t dfs_open(string file_name, string mode);
		int32_t dfs_close(int32_t file_id);
    int32_t dfs_write(int32_t file_id, char* buf, int32_t write_size);
    int32_t dfs_read(int32_t file_id, char* buf, int32_t read_size);

    //TODO: rest of operations
    void dfs_mkdir();
    void dfs_ls();
    void dfs_check_type();

  private:
    int32_t sockfd;
    vector<FileDescriptor*> file_table;
    char client_buffer[MSG_SIZE];
    char tag;
    int32_t size;
    // TODO for ls operation
    //vector<string> ls_vector;
    int32_t local_send();
    int32_t local_recv();
    void dfs_update_write(int32_t file_id, string hash, int32_t block_size);
    int32_t dfs_update_read(int32_t file_id);


};
