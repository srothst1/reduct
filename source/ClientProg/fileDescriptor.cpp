#include "../config.h"
#include "hashObject.h"
#include "fileDescriptor.h"

#include <cstring>
#include <string>
#include <iostream>
#include <cstdint>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <algorithm>

using namespace std;

FileDescriptor::FileDescriptor(string file_name, int32_t file_id, string mode, key_t buf1_key, key_t buf2_key){
  this->file_name = file_name;
  this->file_id = file_id;
  this->block_offset = 0;
  this->offset=0;
  this->mode = mode;
  this->buf1;
  this->buf2;
  //allocate shared memory for buffer
  //if read allocate one buffer. If write alllocate 2 buffers
  if (mode=="r" ){
    int32_t buf1_id = shmget(buf1_key, BLOCK_SIZE*sizeof(char), 0666);
    int32_t buf2_id = 0;
    this->buf1 = (char*) shmat(buf1_id,NULL,0);
    this->buf2 = nullptr;
    this->offset=BLOCK_SIZE;
  }
  else if (mode == "w"){
    int32_t buf1_id = shmget(buf1_key, BLOCK_SIZE*sizeof(char), 0666);
    int32_t buf2_id = shmget(buf2_key, BLOCK_SIZE*sizeof(char), 0666);
    this->buf1 = (char*) shmat(buf1_id,NULL,0);
    this->buf2 = (char*) shmat(buf2_id,NULL,0);
  }


}

FileDescriptor::~FileDescriptor(){
  shmdt(this->buf1);
  if (this->mode=="w"){
    shmdt(this->buf2);
  }
}

int32_t FileDescriptor::getblock_offset(){
  return this->block_offset;
}

int32_t FileDescriptor::getoffset(){
  return this->offset;
}


string FileDescriptor::getfile_name(){
  return this->file_name;
}
string FileDescriptor::getmode(){
  return this->mode;
}

int32_t FileDescriptor::getfile_id(){
  return this->file_id;
}

void FileDescriptor::setblock_offset(int32_t new_offset){
  this->block_offset = new_offset;
}

void FileDescriptor::setoffset(int32_t new_offset){
  this->offset = new_offset;
}

//writes size amount of data into the buffer starting from the current offset
int32_t FileDescriptor::write(char* buf, int32_t size){
  char* dest;
  int32_t copied;

  if (((this->block_offset)%2)+1 == 1){
    dest = this->buf1;
  }
  else{
    dest = this->buf2;
  }
  copied = min(size, BLOCK_SIZE-this->offset);
  memcpy((char*) dest+this->offset, buf, copied*sizeof(char));
  this->offset += copied;

  return copied;
}

//reads size amount of data from the buffer starting from the current offset
int32_t FileDescriptor::read(char* buf, int32_t size){
  int32_t copied = min(size, BLOCK_SIZE-this->offset);
  memcpy(buf, (char*) this->buf1+this->offset, copied*sizeof(char));
  this->offset += copied;
  return copied;
}

//compute hash value of the current buffer
string FileDescriptor::sha_hash(){
  char* src;
  if (((this->block_offset)%2)+1 == 1){
    src = this->buf1;
  }
  else{
    src = this->buf2;
  }
  HashObject new_hash;
  string hash_val = new_hash.Sha1Hash(src, this->offset);
  return hash_val;
}
