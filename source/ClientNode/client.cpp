#include "fileDescriptor.h"
#include "node.h"
#include "client.h"


#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <string>
#include <cstdint>
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>

using namespace std;

Client::Client(int32_t client_num, int32_t sockfd, Node* master_node, mutex* global_mtx1, mutex* global_mtx2, bool* terminate_process){
  this->client_num = client_num;
  this->sockfd = sockfd;
  this->master_node = master_node;
  this->tag = 0;
  this->size = 0;
  this->global_mtx1 = global_mtx1;
  this->global_mtx2 = global_mtx2;
  this->terminate_process = terminate_process;
  this->terminate_thread = false;
  // TODO for ls operation
  //vector<string> ls_vector = ;
}
Client::~Client(){
  for (int32_t i = 0; i< this->file_table.size(); i++){
    if (this->file_table.at(i)!= nullptr){
      this->dfs_close(i);
    }
  }
  if (this->sockfd>-1){
    close(this->sockfd);
  }
}

int32_t Client::getclient_num(){
  return this->client_num;
}

int32_t Client::getsockfd(){
  return this->sockfd;
}

//function for sending a message to the client program
int32_t Client::local_send(){
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

//function for receiving a message from the client program
int32_t Client::local_recv(){
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

//function for handling client program requests
void Client::handle_client_request(){
  bool stop_handle = false;
  int32_t file_id;
  int32_t block_size;
  string block_id;
  while (!stop_handle){
    this->local_mtx.lock();
    stop_handle = this->terminate_thread;
    this->local_mtx.unlock();
    if (this->local_recv()<0){
      this->terminate_client();
      return;
    }

    if (this->tag == OPENR || this->tag ==OPENW){
      this->dfs_open();
    }
    else if (this->tag == CLOSE){
      memcpy(&file_id, this->client_buffer, sizeof(int32_t));
      this->dfs_close(file_id);
    }
    else if (this->tag == WRITE){
      //assign file_id to local variable
      memcpy(&file_id, this->client_buffer, sizeof(int32_t));

      //receive block_size & assign to local variable
      if (this->local_recv()<0){
        this->terminate_client();
        return;
      }
      memcpy(&block_size, this->client_buffer, sizeof(int32_t));
      //receive block_id and assign to local variable
      if (this->local_recv()<0){
        this->terminate_client();
        return;
      }
      block_id = this->client_buffer;
      this->dfs_write(file_id, block_size, block_id);
    }
    else if (this->tag == READ){
      this->dfs_read();
    }
    //TODO: add following functions
    else if (this->tag == MKDIR){
      this->dfs_mkdir();
    }
    else if (this->tag == LS){
      this->dfs_ls();
    }
    else if (this->tag == CHECK){
      this->dfs_check_type();
    }
    else {
      this->terminate_client();
      return;
    }
    if(this->local_send()<0){
      this->terminate_client();
      return;
    }
  }
}

//function for terminaing the entire ClientNode process
void Client::terminate_all(){
  this->terminate_client();
  this->global_mtx1->lock();
  *this->terminate_process = true;
  this->global_mtx1->unlock();

}

//function for terminating the client thread
void Client::terminate_client(){
  //set terminate thread to true, so the handle_client_request thread can terminate
  this->local_mtx.lock();
  this->terminate_thread = true;
  this->local_mtx.unlock();
  if (this->sockfd >-1){
    close(this->sockfd);
    this->sockfd=-1;
  }
  //close the files
  for (int32_t i = 0; i< this->file_table.size(); i++){
    if (this->file_table.at(i)!= nullptr){
      this->dfs_close(i);
    }
  }
  this->file_table.clear();
  this->file_table.shrink_to_fit();

}

//function opening a file
void Client::dfs_open(){
  string file_name = this->client_buffer;
  string mode;
  int32_t file_id;
  if (this->tag == OPENW){
    mode = "w";
  }
  else if (this->tag ==OPENR){
    mode = "r";
  }

  //send the master node file name and receive COMPLETE or OP_FAIL message
  this->global_mtx2->lock();
  if (this->master_node->dfs_send(this->tag, (int32_t) file_name.size()+1, file_name.c_str())< 0){
    this->terminate_all();
    this->global_mtx2->unlock();
    return;
  }
  if (this->master_node->dfs_recv(&this->tag, &this->size, this->client_buffer)< 0){
    this->terminate_all();
    this->global_mtx2->unlock();
    return;
  }
  this->global_mtx2->unlock();

  if (this->tag == COMPLETE){
    //initialize a new file descriptor for the file
    key_t buf1_key = ftok(".",this->file_table.size()*2+this->client_num*512);
    key_t buf2_key = ftok(".",this->file_table.size()*2+this->client_num*512+1);
    FileDescriptor* new_descriptor = new FileDescriptor(file_name, mode, buf1_key, buf2_key);
    this->size = sizeof(key_t);
    memcpy(this->client_buffer, &buf1_key, sizeof(key_t));
    if(this->local_send()<0){
      this->terminate_client();
      return;
    }

    if (mode == "w"){
      this->size = sizeof(key_t);
      memcpy(this->client_buffer, &buf2_key, sizeof(key_t));
      if(this->local_send()<0){
        this->terminate_client();
        return;
      }
    }
    file_id = file_table.size();
    this->file_table.push_back(new_descriptor);
    this->size = sizeof(int32_t);
    memcpy(this->client_buffer, &file_id, sizeof(int32_t));
  }
  else{
    this->size = sizeof(key_t);

  }

}

//function for closing a file
void Client::dfs_close(int32_t file_id){
  string file_name;
  if ( this->file_table.size()<= file_id || file_id < 0|| this->file_table.at(file_id)==nullptr){
    this->tag = OP_FAIL;
  }
  else{
    file_name = this->file_table.at(file_id)->getfile_name();
    this->global_mtx2->lock();
    if (this->master_node->dfs_send(this->tag, (int32_t) file_name.size()+1, file_name.c_str()) < 0) {
      this->terminate_all();
      this->global_mtx2->unlock();
      return;
    }
    if (this->master_node->dfs_recv(&this->tag, &this->size, this->client_buffer) < 0){
      this->terminate_all();
      this->global_mtx2->unlock();
      return;
    }
    this->global_mtx2->unlock();
    delete this->file_table.at(file_id);
    file_table.at(file_id)= nullptr;
  }
}

//function for writing a data block into the file system
void Client::dfs_write(int32_t file_id, int32_t block_size, string block_id){
  string file_name;
  int32_t block_offset;
  string ip;
  FileDescriptor* file_descriptor;
  int32_t sent_size = 0;
  char* block_buffer;

  if ( this->file_table.size()<= file_id){
    this->tag = OP_FAIL;
  }
  else{
    file_descriptor = this->file_table.at(file_id);
    file_name = file_descriptor->getfile_name();
    block_offset = file_descriptor->getblock_offset();
    block_buffer = file_descriptor->getbuf((block_offset%2)+1);

    this->global_mtx2->lock();
    if (this->master_node->dfs_send(WRITE, (int32_t) block_id.size()+1,block_id.c_str())< 0) {
      this->terminate_all();
      this->global_mtx2->unlock();
      return;
    }
    if (this->master_node->dfs_recv(&this->tag, &this->size, this->client_buffer)< 0) {
      this->terminate_all();
      this->global_mtx2->unlock();
      return;
    }
    this->global_mtx2->unlock();

    if (this->tag == COMPLETE || this->tag == DUP ){
      ip = this->client_buffer;
      try{
        // initialize data_node
        Node* data_node = new Node(ip);
        //sends block ID
        if (data_node->dfs_send(WRITE, (int32_t) block_id.size()+1, block_id.c_str())<0){
          this->tag = OP_FAIL;
          delete data_node;
          return;
        }
        //sends data block
        this->size = (int32_t) min(block_size-sent_size, BLOCK_SIZE);
        if(block_size-sent_size <= this->size){
          this->tag = COMPLETE;
        }
        else{
          this->tag = MORE;
        }
        if (data_node->dfs_send(this->tag, this->size, block_buffer)<0){
          this->tag = OP_FAIL;
          delete data_node;
          return;
        }
        sent_size += this->size;
        while (block_size-sent_size>0){
          this->size = (int32_t) min(block_size-sent_size, BLOCK_SIZE);
          if(block_size-sent_size <= this->size){
            this->tag = COMPLETE;
          }
          else{
            this->tag = MORE;
          }
          if (data_node->dfs_send(this->tag, this->size, (char*) block_buffer + sent_size)<0){
            this->tag = OP_FAIL;
            delete data_node;
            return;
          }
          sent_size += this->size;
        }

        if (data_node->dfs_recv(&this->tag, &this->size, this->client_buffer)<0){
          this->tag = OP_FAIL;
          delete data_node;
          return;
        }

        if (this->tag == NOTDUP){
          //increment the block_id by 1 and try writing again
          string newblock_id = block_id.substr(0,block_id.size()-1)+
          to_string(stoi(block_id.substr(block_id.size()-1,1),nullptr,16)+1);
          this->dfs_write(file_id, block_size, newblock_id);
        }
        //send tag, offset, size, block ID to the master node in order and receive
        //a complete message
        else if (this->tag == COMPLETE || this->tag == DUP){
          this->global_mtx2->lock();
          if (this->master_node->dfs_send(this->tag, (int32_t) file_name.size()+1, file_name.c_str())< 0) {
            this->terminate_all();
            this->global_mtx2->unlock();
            return;
          }
          memcpy(this->client_buffer, &block_offset, sizeof(int32_t));
          if (this->master_node->dfs_send(OFFSET, (int32_t) sizeof(int32_t) , this->client_buffer)< 0) {
            this->terminate_all();
            this->global_mtx2->unlock();
            return;
          }
          memcpy(this->client_buffer, &block_size, sizeof(int32_t));
          if (this->master_node->dfs_send(SIZE, (int32_t) sizeof(int32_t), this->client_buffer)< 0) {
            this->terminate_all();
            this->global_mtx2->unlock();
            return;
          }
          if (this->master_node->dfs_send(ID, (int32_t) block_id.size()+1, block_id.c_str())< 0) {
            this->terminate_all();
            this->global_mtx2->unlock();
            return;
          }
          if (this->master_node->dfs_recv(&this->tag, &this->size, this->client_buffer)< 0) {
            this->terminate_all();
            this->global_mtx2->unlock();
            return;
          }
          this->global_mtx2->unlock();
          file_descriptor->setblock_offset(block_offset+1);
        }

        this->tag = COMPLETE;
        delete data_node;
      }
      catch(exception& e){
        this->tag = OP_FAIL;
      }
    }
    else {
      this->tag = OP_FAIL;
    }

  }
}

//function for reading a data block
void Client::dfs_read(){
  int32_t file_id;
  string file_name;
  int32_t block_offset;
  string block_id;
  string ip;
  FileDescriptor* file_descriptor;
  int32_t block_size = 0;
  char* block_buffer;
  //get file id
  memcpy(&file_id, this->client_buffer, sizeof(int32_t));
  if ( this->file_table.size()<= file_id|| this->file_table.at(file_id) == nullptr){
    this->tag = OP_FAIL;
  }
  else{
    //get file_descriptor, block_offset, buffer to receive data
    file_descriptor = this->file_table.at(file_id);
    file_name = file_descriptor->getfile_name();
    block_offset = file_descriptor->getblock_offset();
    block_buffer = file_descriptor->getbuf(1);

    //send master_node filename, block offset in order and receive data node ip
    //and if applicable, block_id in order
    this->global_mtx2->lock();
    if (this->master_node->dfs_send(READ, file_name.size()+1,file_name.c_str())<0){
      this->terminate_all();
      this->global_mtx2->unlock();
      return;
    }
    memcpy(this->client_buffer, &block_offset, sizeof(int32_t));
    //send block offset
    if (this->master_node->dfs_send(OFFSET, sizeof(int32_t),this->client_buffer)<0){
      this->terminate_all();
      this->global_mtx2->unlock();
      return;
    }
    //receive datanode IP
    if (this->master_node->dfs_recv(&this->tag, &this->size,this->client_buffer)<0){
      this->terminate_all();
      this->global_mtx2->unlock();
      return;
    }
    if (this->tag == COMPLETE){
      ip = this->client_buffer;
      if (this->master_node->dfs_recv(&this->tag, &this->size,this->client_buffer) < 0){
        this->terminate_all();
        this->global_mtx2->unlock();
        return;
      }
    }
    this->global_mtx2->unlock();

    if (this->tag == ID){
      //initialize block_id
      block_id = this->client_buffer;
      try{
        //Create data_node
        Node* data_node = new Node(ip);
        //send the block_id to data node
        if (data_node->dfs_send(READ, (int32_t) block_id.size()+1, block_id.c_str())<0){
          this->tag = OP_FAIL;
          delete data_node;
          return;
        }
        //receive first block of data
        if (data_node->dfs_recv(&this->tag, &this->size, block_buffer)<0){
          this->tag = OP_FAIL;
          delete data_node;
          return;
        }
        block_size += this->size;
        //receive more blocks of data if there is
        while (this->tag == MORE){
          if (data_node->dfs_recv(&this->tag, &this->size, (char*) block_buffer + block_size)<0){
            this->tag = OP_FAIL;
            delete data_node;
            return;
          }
          block_size += this->size;
        }
        this->tag = COMPLETE;
        //copy block size into the client_buffer to send back
        memcpy(this->client_buffer, &block_size,sizeof(int32_t));
        //increment the block offset
        file_descriptor->setblock_offset(block_offset+1);
        delete data_node;
      }
      catch(exception& e){
        this->tag = OP_FAIL;
      }
    }
    else{
      this->tag = OP_FAIL;
    }

  }
  //set the size to int_32t
  this->size = sizeof(int32_t);
}

//TODO: implement following functions
void Client::dfs_mkdir(){

}

void Client::dfs_ls(){

}

void Client::dfs_check_type(){

}
