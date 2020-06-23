#include "secureMap.h"
#include "fileDirectory.h"
#include "node.h"
#include "client.h"

#include <sstream>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <string>
#include <cstdint>
#include <ctime>
#include <vector>
#include <mutex>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

using namespace std;

Client::Client(Node* client_node, FileDirectory* filedirectory,SecureMap<string, int32_t>*blockMAP,
  SecureMap<string,string>*groupMAP, vector<Node*>* data_nodes, mutex* global_mtx){
  this->client_node = client_node;
  this->tag = 0;
  this->size = 0;
  this->filedirectory = filedirectory;
  this->blockMAP = blockMAP;
  this->groupMAP = groupMAP;
  this->data_nodes = data_nodes;
  this->global_mtx = global_mtx;
  // TODO for ls operation
  //vector<string> ls_vector = ;
}
Client::~Client(){
  delete this->client_node;
}

//function for handling client requests
void Client::handle_client_request(){
  bool stop_handle = false;
  int32_t file_id;
  int32_t block_size;
  string block_id;
  while (!stop_handle){
    this->local_mtx.lock();
    stop_handle = this->terminate_thread;
    this->local_mtx.unlock();
    if (this->client_node->dfs_recv(&this->tag, &this->size, this->client_buffer) < 0){
      this->terminate_client();
      return;
    }

    if (this->tag == OPENR || this->tag ==OPENW){
      this->dfs_open();
    }
    else if (this->tag == CLOSE){
      this->dfs_close();
    }
    else if (this->tag == WRITE){
      this->dfs_write();
    }
    else if (this->tag == READ){
      this->dfs_read();
    }
    else if (this->tag == COMPLETE || this->tag == DUP){
      this->dfs_complete();
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
  }
}

//function for terminating the client thread
void Client::terminate_client(){
  //set terminate thread to true, so the handle_client_request thread can terminate
  this->local_mtx.lock();
  this->terminate_thread = true;
  this->local_mtx.unlock();
}

vector<string> Client::parse_directory(){
  vector<string> dir_name;
  string dir = this->client_buffer;
  stringstream str_stream(dir);
  string intermediate;

  while (getline(str_stream, intermediate, '/')) {
    dir_name.push_back(intermediate);
  }
  return dir_name;
}

//function for opening file
void Client::dfs_open(){
  string file_name = this->client_buffer;
  string mode;
  int32_t file_id;
  bool success;
  if (this->tag == OPENW){
    mode = "w";
  }
  else if (this->tag ==OPENR){
    mode = "r";
  }
  vector<string> dir_name = this->parse_directory();
  for (int i =0 ; i < dir_name.size(); i++){
  }

  if (filedirectory->open(dir_name, mode)){
    this->tag = COMPLETE;
  }
  else{
    this->tag = OP_FAIL;
  }

  if (this->client_node->dfs_send(this->tag, (int32_t) sizeof(int32_t), this->client_buffer)< 0){
    this->terminate_client();
    return;
  }

}

//function for closing file
void Client::dfs_close(){
  if (this->client_node->dfs_send(COMPLETE, (int32_t) sizeof(int32_t), this->client_buffer) < 0) {
    this->terminate_client();
    return;
  }
  return;
}

//function for writing data block. Adds the block ID to BlockMap and group to
//GroupMap if necessary
void Client::dfs_write(){
  string block_id = this->client_buffer;
  string block_group = block_id.substr(0,2);
  string ip = "";
  if (!this->blockMAP->contains(block_id)){
    this->blockMAP->insert(block_id, (int32_t) 0);
  }
  if (!this->groupMAP->contains(block_group)){
    srand (time(NULL));
    ip = (this->data_nodes->at(rand() % this->data_nodes->size()))->getip_address();
    this->groupMAP->insert(block_group, ip);
  }
  else{
    ip = this->groupMAP->get(block_group);
  }
  if (this->client_node->dfs_send(COMPLETE, (int32_t) ip.size()+1, ip.c_str())< 0) {
    this->terminate_client();
    return;
  }
  return;
}

//function for updating metadata after the client wrote data block
void Client::dfs_complete(){
  char dup = this->tag;
  int32_t block_offset;
  int32_t block_size;
  string block_id;
  vector<string> dir_name = this->parse_directory();

  if (this->client_node->dfs_recv(&this->tag, &this->size, this->client_buffer)< 0) {
    this->terminate_client();
    return;
  }
  if (this->tag == OFFSET){
    memcpy(&block_offset, this->client_buffer, sizeof(int32_t));
  }
  else{
    this->terminate_client();
    return;
  }

  if (this->client_node->dfs_recv(&this->tag, &this->size, this->client_buffer)< 0) {
    this->terminate_client();
    return;
  }
  if (this->tag ==SIZE){
    memcpy(&block_size, this->client_buffer, sizeof(int32_t));
  }
  else{
    this->terminate_client();
    return;
  }

  if (this->client_node->dfs_recv(&this->tag, &this->size, this->client_buffer)< 0) {
    this->terminate_client();
    return;
  }
  if (this->tag ==ID){
    block_id = this->client_buffer;
  }
  else{
    this->terminate_client();
    return;
  }
  this->tag = COMPLETE;

  if (!this->filedirectory->setblockID(dir_name, block_offset, block_id)){
    this->tag = OP_FAIL;
  }
  this->global_mtx->lock();
  if (blockMAP->contains(block_id)){
    int32_t num_dup = blockMAP->get(block_id);
    blockMAP->update(block_id, num_dup+1);
  }
  this->global_mtx->unlock();


  this->global_mtx->lock();
  long size = ((long) block_size) + this->filedirectory->gettotal_size();
  filedirectory->settotal_size(size);
  this->global_mtx->unlock();

  if (dup == DUP){
    this->global_mtx->lock();
    size = ((long) block_size)+ this->filedirectory->getduplicate_size();
    filedirectory->setduplicate_size(size);
    this->global_mtx->unlock();
  }

  if (this->client_node->dfs_send(COMPLETE, (int32_t) sizeof(int32_t) , this->client_buffer)< 0) {
    this->terminate_client();
    return;
  }
  return;
}

//function for reading data block. Retrieves the data block location for given
//file name and offset and sends it to the client node
void Client::dfs_read(){
  int32_t block_offset;
  string block_id;
  string ip;
  vector<string> dir_name = this->parse_directory();

  if (this->client_node->dfs_recv(&this->tag, &this->size, this->client_buffer)< 0) {
    this->terminate_client();
    return;
  }
  if (this->tag ==OFFSET){
    memcpy(&block_offset, this->client_buffer, sizeof(int32_t));
  }
  else{
    this->terminate_client();
    return;
  }

  block_id = this->filedirectory->getblockID(dir_name, block_offset);
  if (block_id == ""){
    this->tag = OP_FAIL;
    this->size = sizeof(int32_t);
  }
  else{
    ip = this->groupMAP->get(block_id.substr(0,2));
    this->tag = COMPLETE;
    this->size = (int32_t) ip.size()+1;
    memcpy(this->client_buffer,ip.c_str(),ip.size()+1);
  }
  if (this->client_node->dfs_send(this->tag, this->size, this->client_buffer)< 0) {
    this->terminate_client();
    return;
  }

  if (block_id != ""){
    if (this->client_node->dfs_send(ID, block_id.size()+1, block_id.c_str())< 0) {
      this->terminate_client();
      return;
    }
  }

}

//TODO: implement following functions
void Client::dfs_mkdir(){

}

void Client::dfs_ls(){

}

void Client::dfs_check_type(){

}
