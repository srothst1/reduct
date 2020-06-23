#include "../config.h"
#include "fileDescriptor.h"

#include <string>
#include <iostream>
#include <cstdint>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

using namespace std;

FileDescriptor::FileDescriptor(string file_name, string mode, key_t buf1_key, key_t buf2_key){
  this->file_name = file_name;
  this->block_offset = 0;
  this->mode = mode;
  this->buf1_key = buf1_key;
  this->buf2_key = buf2_key;
  //allocate shared memory for buffer
  //if read allocate one buffer. If write alllocate 2 buffers
  if (mode=="r" ){
    this->buf1_id = shmget(buf1_key, BLOCK_SIZE*sizeof(char), IPC_CREAT | 0666);
    this->buf2_id = 0;
    this->buf1 = (char*) shmat(this->buf1_id,NULL,0);
    this->buf2 = nullptr;
  }
  else if (mode == "w"){
    this->buf1_id = shmget(buf1_key, BLOCK_SIZE*sizeof(char), IPC_CREAT | 0666);
    this->buf2_id = shmget(buf2_key, BLOCK_SIZE*sizeof(char), IPC_CREAT | 0666);
    this->buf1 = (char*) shmat(this->buf1_id,NULL,0);
    this->buf2 = (char*) shmat(this->buf2_id,NULL,0);
  }


}

FileDescriptor::~FileDescriptor(){
  shmdt(this->buf1);
  shmctl(this->buf1_id, IPC_RMID, NULL);
  if (this->mode=="w"){
    shmdt(this->buf2);
    shmctl(this->buf2_id, IPC_RMID, NULL);
  }
}

int32_t FileDescriptor::getblock_offset(){
  return this->block_offset;
}
string FileDescriptor::getfile_name(){
  return this->file_name;
}
string FileDescriptor::getmode(){
  return this->mode;
}
key_t FileDescriptor::getbuf_key(int32_t buf){
  if (buf ==1){
    return this->buf1_key;
  }
  else {
    return this->buf2_key;
  }
}

char* FileDescriptor::getbuf(int32_t buf){
  if (buf ==1){
    return this->buf1;
  }
  else {
    return this->buf2;
  }
}
void FileDescriptor::setblock_offset(int32_t offset){
  this->block_offset = offset;
}
