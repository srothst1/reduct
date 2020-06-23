
#include "../config.h"
#include "fileDescriptor.h"
#include "fileObject.h"

#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <string>
#include <cstdint>
#include <vector>
#include <thread>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

using namespace std;

FileObject::FileObject(string ip){
  struct sockaddr_in dfs_addr;
  if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
      throw invalid_argument("Socket creation failed");
  }
  dfs_addr.sin_port =  htons(PORTNUM2);
  dfs_addr.sin_family =  AF_INET;
  if(inet_pton(AF_INET, ip.c_str(), &dfs_addr.sin_addr)<=0){
      throw invalid_argument("Invalid address/ Address not supported");
  }
  if (connect(this->sockfd, (struct sockaddr *)&dfs_addr, sizeof(dfs_addr)) < 0){
      throw invalid_argument("Invalid address/ Address not supported");
  }
  this->tag=0;
  this->size=0;
  // TODO for ls operation
  //vector<string> ls_vector = ;
}
FileObject::~FileObject(){
  for (int32_t i = 0; i< this->file_table.size(); i++){
    if (this->file_table.at(i)!= nullptr){
      this->dfs_close(i);
    }
  }
  if (this->sockfd>-1){
    close(this->sockfd);
  }
}


int32_t FileObject::getsockfd(){
  return this->sockfd;
}

//function for sending a message to CLientNode process
int32_t FileObject::local_send(){
  if (send(this->sockfd, &this->tag, sizeof(char), MSG_NOSIGNAL) <1){
    return -1;
  }
  if (send(this->sockfd, &this->size, sizeof(int32_t), MSG_NOSIGNAL) <1){
    return -1;
  }
  if (send(this->sockfd, this->client_buffer, sizeof(char)*this->size, MSG_NOSIGNAL) <1){
    return -1;
  }
  return 0;
}

//function for receiving a message from ClientNode process
int32_t FileObject::local_recv(){
  if (recv(this->sockfd, &this->tag, sizeof(char), MSG_WAITALL) <1){
    return -1;
  }
  if (recv(this->sockfd, &this->size, sizeof(int32_t), MSG_WAITALL) <1){
    return -1;
  }
  if (recv(this->sockfd, this->client_buffer, sizeof(char)*this->size, MSG_WAITALL) <1){
    return -1;
  }
  return 0;
}

//function for opening file
int32_t FileObject::dfs_open(string file_name, string mode){
  key_t key1= 0;
  key_t key2= 0;
  int32_t file_id;

  if (mode == "r"){
    this->tag = OPENR;
  }
  else if (mode == "w"){
    this->tag = OPENW;
  }
  else{
    return -1;
  }
  //sends file name
  this->size = file_name.size()+1;
  memcpy(this->client_buffer, file_name.c_str(), (file_name.size()+1)* sizeof(char));
  if (this->local_send()<0){
    return -1;
  }

  if (this->local_recv()<0){
    return -1;
  }
  if (this->tag == COMPLETE){
    //returns file descriptor (int)
    memcpy(&key1, this->client_buffer, sizeof(key_t));
    if (mode == "w"){
      if (this->local_recv()<0){
        return -1;
      }
      memcpy(&key2, this->client_buffer, sizeof(key_t));
    }
    if (this->local_recv()<0){
      return -1;
    }
    memcpy(&file_id, this->client_buffer, sizeof(int32_t));
    file_table.push_back(new FileDescriptor(file_name, file_id, mode, key1, key2));
    return file_table.size()-1;
  }
  return -1;
}

//function for closing file
int32_t FileObject::dfs_close(int32_t file_id){
  if (file_id < 0 || file_id >= this->file_table.size()){
    return -1;
  }
  FileDescriptor* file = this->file_table.at(file_id);
  int32_t sys_file_id = file->getfile_id();
  int32_t last_block = file->getoffset();
  if (file->getmode()=="w"){
    string hash_val = file->sha_hash();
    this->dfs_update_write(sys_file_id, hash_val, last_block);
  }
  this->tag = CLOSE;
  this->size = sizeof(int32_t);
  memcpy(this->client_buffer, &sys_file_id, sizeof(int32_t));
  if (this->local_send()<0){
    return -1;
  }
  if (this->local_recv()<0){
    return -1;
  }
  delete this->file_table.at(file_id);
  file_table.at(file_id)= nullptr;

  if (this->tag == COMPLETE){
    return 0;
  }

  return -1;
}

//function for writing given data into buffer
int32_t FileObject::dfs_write(int32_t file_id, char* buf, int32_t write_size){
  if (file_id < 0 || file_id >= this->file_table.size()){
    return -1;
  }
  char* curbuf = buf;
  int32_t remain_size= write_size;
  int32_t block_size, copied_size;
  FileDescriptor* file = this->file_table.at(file_id);
  string hash_val;
  int32_t fst = 0;
  int32_t buf_remain_size;
  thread* update;

  //keep writing into the buffer until all data is written
  while(remain_size > 0){
    block_size = min(BLOCK_SIZE, remain_size);
    copied_size = file->write(curbuf, block_size);
    remain_size -= copied_size;
    curbuf = (char*) curbuf + copied_size;
    if (copied_size < block_size){
      if (fst==0){
        hash_val = file->sha_hash();
        fst++;
      }
      //if the end of the buffer is called, update thread is called and the process
      //keeps writing into the second buffer
      update = new thread(&FileObject::dfs_update_write, this, file->getfile_id(), hash_val, BLOCK_SIZE);
      buf_remain_size = BLOCK_SIZE;
      file->setblock_offset(file->getblock_offset()+1);
      file->setoffset(0);
      while(buf_remain_size>0 && remain_size > 0){
        block_size = min(BLOCK_SIZE, remain_size);
        copied_size = file->write(curbuf, block_size);
        remain_size -= copied_size;
        buf_remain_size -= copied_size;
        curbuf = (char*) curbuf + copied_size;
      }
      if (buf_remain_size <=0){
        hash_val = file->sha_hash();
      }
      update->join();
      delete update;
      if (this-> tag != COMPLETE){
        file->setblock_offset(file->getblock_offset()-1);
        file->setoffset(BLOCK_SIZE);
        curbuf = (char*) curbuf - (BLOCK_SIZE-buf_remain_size);
        return write_size-remain_size-(BLOCK_SIZE-buf_remain_size);
      }
    }

  }
  return write_size;

}

int32_t FileObject::dfs_read(int32_t file_id, char* buf, int32_t read_size){
  if (file_id < 0 || file_id >= this->file_table.size()){
    return -1;
  }

  char* curbuf = buf;
  int32_t remain_size= read_size;
  int32_t block_size, copied_size;
  FileDescriptor* file = this->file_table.at(file_id);
  this->tag = COMPLETE;
  //keep reading data until requested amount of data is read or it reaches the
  //end of the file
  while(remain_size > 0 && this->tag != OP_FAIL){
    block_size = min(BLOCK_SIZE, remain_size);
    copied_size = file->read(curbuf, block_size);
    remain_size -= copied_size;
    curbuf = (char*) curbuf + copied_size;
    //if the end of the buffer is reached, update operation is called to update the
    //buffer
    if (copied_size < block_size){
      int32_t updated_size = this->dfs_update_read(file->getfile_id());
      if (this->tag==COMPLETE){
        file->setblock_offset(file->getblock_offset()+1);
        file->setoffset(BLOCK_SIZE - updated_size);
      }
    }

  }
  return (read_size - remain_size);
}

//function to update the buffer for writing
void FileObject::dfs_update_write(int32_t file_id, string hash, int32_t block_size){
  if (block_size <= 0){
    return;
  }
  //genereates block ID by concatenating hash value with 0
  string block_id = hash+"0";
  //sends the file ID
  this->tag = WRITE;
  this->size = sizeof(int32_t);
  memcpy(this->client_buffer, &file_id, sizeof(int32_t));
  if (this->local_send()<0){
    this->tag = OP_FAIL;
    return;
  }

  //sends the size
  this->tag = SIZE;
  this->size = sizeof(int32_t);
  memcpy(this->client_buffer, &block_size, sizeof(int32_t));
  if (this->local_send()<0){
    this->tag = OP_FAIL;
    return;
  }

  //sends the block ID
  this->tag = ID;
  this->size = block_id.size()+1;
  memcpy(this->client_buffer, block_id.c_str(), sizeof(char)*(block_id.size()+1));
  if (this->local_send()<0){
    this->tag = OP_FAIL;
    return;
  }

  if (this->local_recv()<0){
    this->tag = OP_FAIL;
    return;
  }

  return;
}

//function for updating the buffer for reading
int32_t FileObject::dfs_update_read(int32_t file_id){
  int32_t block_size;
  this->tag = READ;
  this->size = sizeof(int32_t);
  memcpy(this->client_buffer, &file_id, sizeof(int32_t));
  if (this->local_send()<0){
    return 0;
  }

  if (this->local_recv()<0){
    return 0;
  }
  if (this->tag == COMPLETE){
    memcpy(&block_size, this->client_buffer, sizeof(int32_t));
    return block_size;
  }
  return 0;

}

//TODO: rest of operations
void FileObject::dfs_mkdir(){

}
void FileObject::dfs_ls(){

}
void FileObject::dfs_check_type(){

}
