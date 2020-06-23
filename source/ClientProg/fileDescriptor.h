#pragma once

#include <string>
#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

/*Class for file descriptor of a client. File descriptor stores the
file name, block_offset for the curren buffer, and 2 buffers for writing
and 1 buffer for reading data. Buffer is shared memory with the ClientNode process*/
class FileDescriptor {
  public:
    FileDescriptor(string file_name, int32_t file_id,string mode, key_t buf1_key, key_t buf2_key);
    ~FileDescriptor();
    int32_t getblock_offset();
    int32_t getoffset();
    string getfile_name();
    int32_t getfile_id();
    string getmode();
    int32_t write(char* buf, int32_t size);
    int32_t read(char* buf, int32_t size);
    void setblock_offset(int32_t new_offset);
    void setoffset(int32_t new_offset);
    string sha_hash();

  private:
    string file_name;
    int32_t file_id;
    int32_t block_offset;
    int32_t offset;
    string mode;
    char* buf1;
    char* buf2;
};
