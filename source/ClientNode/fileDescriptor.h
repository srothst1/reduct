#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

using namespace std;

/*Class for file descriptor of a client. File descriptor stores the
file name, block_offset for the curren buffer, and 2 buffers for writing
and 1 buffer for reading data. Buffer is shared memory with the client program*/
class FileDescriptor {
  public:
    FileDescriptor(string file_name, string mode, key_t buf1_key, key_t buf2_key);
    ~FileDescriptor();
    int32_t getblock_offset();
    string getfile_name();
    string getmode();
    key_t getbuf_key(int32_t buf);
    char* getbuf(int32_t buf);
    void setblock_offset(int32_t offset);

  private:
    string file_name;
    int32_t block_offset;
    string mode;
    char* buf1;
    char* buf2;
    key_t buf1_key;
    key_t buf2_key;
    int32_t buf1_id;
    int32_t buf2_id;

};
