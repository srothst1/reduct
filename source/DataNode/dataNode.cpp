#include "../config.h"
#include "node.h"
#include "dataNode.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>


using namespace std;

DataNode::DataNode(string master_node_ip){
  this->master_node_ip = master_node_ip;
  this->listenfd = 0;
  this->terminate_process = false;
  this->master_node = nullptr;

}

DataNode::~DataNode(){
  for (int i = 0 ; i < this->thread_list.size(); i++){
    delete thread_list.at(i);
  }
}

int32_t DataNode::getlistenfd(){
  return this->listenfd;
}

string DataNode::getmaster_node_ip(){
  return this->master_node_ip;
}

//listener thread that acceps new connection from a client node
void DataNode::listener(){
  int32_t sockfd;
  unsigned int socklen;
  struct sockaddr_in listen_addr, client_addr, datanode_addr, masternode_addr;
  struct linger linger_val;
  int opt = 1;
  bool terminate = this->terminate_process;
  //connect to master node
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("Socket creation failed\n");
    return;
  }
  masternode_addr.sin_port =  htons(PORTNUM2);
  masternode_addr.sin_family =  AF_INET;
  if(inet_pton(AF_INET, this->master_node_ip.c_str(), &masternode_addr.sin_addr)<=0){
    perror("Invalid address/ Address not supported\n");
    return;
  }
  if (connect(sockfd, (struct sockaddr *)&masternode_addr, sizeof(masternode_addr)) < 0){
    perror("Invalid address/ Address not supported\n");
    return;
  }
  //getting own ip address
  socklen_t datanode_addrlen = sizeof(datanode_addr);
  getsockname(sockfd, (struct sockaddr*)&datanode_addr, &datanode_addrlen);
  const char* p = inet_ntop(AF_INET, &datanode_addr.sin_addr, this->buffer, MSG_SIZE);
  if (p == NULL){
    perror("Invalid address/ Address not supported\n");
    return;
  }

  this->ip = this->buffer;
  this->master_node = new Node(sockfd);
  if (this->master_node->dfs_send(IP, (int32_t) this->ip.size()+1, this->ip.c_str())<0){
    return;
  }
  this->master_node->setip_address(this->master_node_ip);

  //set up listener for clients
  if(!(this->listenfd = socket(AF_INET, SOCK_STREAM, 0))) {
    perror("Create Socket Failed\n");
    return;
  }

  //set linger values off for listenfd
  linger_val.l_onoff = 0;
  linger_val.l_linger = 0;
  if(setsockopt(this->listenfd, SOL_SOCKET, SO_LINGER, (void *)&linger_val,
      (socklen_t) (sizeof(struct linger)))<0) {
    perror("Setting socket option failed\n");
    return;
  }

  // set SO_REUSEADDR on listenfd to true
  opt = 1;
  if (setsockopt(this->listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))<0){
    perror("Setting socket option failed\n");
    return;
  }

  listen_addr.sin_family =  AF_INET;
  listen_addr.sin_port =  htons(PORTNUM);
  listen_addr.sin_addr.s_addr = INADDR_ANY;

  //bind the listenfd to the port
  if (bind(this->listenfd, (struct sockaddr *)&listen_addr, sizeof(listen_addr))<0){
    perror("Binding failed\n");
    return;
  }

  //set listenfd to listen
  if(listen(listenfd, 10) <0){
    perror("Binding failed\n");
    close(listenfd);
    return;
  }

  socklen = sizeof(client_addr);

  //while loop leave listening port open to accept a client
  while (!terminate){
    sockfd = accept(this->listenfd, (struct sockaddr *) &client_addr, &socklen);
    if (sockfd<0) {
      perror("TCP connnection with the client failed\n");
    }
    else{
      Node* new_client = new Node(sockfd);
      thread* new_thread = new thread(&DataNode::handle_client_request, this, new_client);
      this->thread_list.push_back(new_thread);
      //cout << "new client joined " << endl;
    }
    this->mtx.lock();
    terminate = this->terminate_process;
    this->mtx.unlock();

  }

}

//thread for handling client node requests
void DataNode::handle_client_request(Node* client){
  char tag;
  int32_t size;
  char* msg_buffer = new char[MSG_SIZE];
  string block_ID;
  client->dfs_recv(&tag, &size, msg_buffer);
  block_ID = msg_buffer;
  if (tag == READ){
    this->dfs_read(client, block_ID);
  }
  else if(tag == WRITE){
    this->dfs_write(client, block_ID);
  }

  delete client;
  delete [] msg_buffer;
}

//function for reading a data block requested by a client node
void DataNode::dfs_read(Node* client, string block_ID){
  char tag;
  int32_t size;
  string file_path = "/local/dfs/" + block_ID.substr(0,1) +"/"+ block_ID.substr(1,1) + "/"+  block_ID;
  char* block_buffer = new char[BLOCK_SIZE];
  ifstream block_file(file_path, ios::binary);
  if (!block_file.is_open()){
    delete [] block_buffer;
    return;
  }
  block_file.read(block_buffer, BLOCK_SIZE*sizeof(char));
  tag = COMPLETE;
  size = block_file.gcount();
  client->dfs_send(tag, size, block_buffer);
  delete [] block_buffer;
}

//function for writing data block received from a client node
void DataNode::dfs_write(Node* client, string block_ID){
  char tag;
  int32_t size;
  char* block_buffer1 = new char[BLOCK_SIZE];
  char* block_buffer2 = nullptr;
  string file_path = "/local/dfs/" + block_ID.substr(0,1) +"/"+ block_ID.substr(1,1) +"/"+ block_ID;
  mkdir(file_path.substr(0, 10).c_str(), S_IRWXU);
  mkdir(file_path.substr(0, 12).c_str(), S_IRWXU);
  mkdir(file_path.substr(0, 14).c_str(), S_IRWXU);
  ifstream check_file(file_path, ios::binary);
  if (client->dfs_recv(&tag, &size, block_buffer1)<0){
    delete [] block_buffer1;
    return;
  }
  if (check_file.is_open()){
    block_buffer2 = new char[BLOCK_SIZE];
    check_file.read(block_buffer2, size*sizeof(char));
    size = min(size, (int32_t) check_file.gcount());

    if (!memcmp(block_buffer1, block_buffer2, size)){
      tag = DUP;
    }
    else{
      tag = NOTDUP;
    }
  }
  else{
    ofstream block_file(file_path, ios::binary);
    block_file.write(block_buffer1, (int32_t) size*sizeof(char));
    tag = COMPLETE;
    block_file.close();
  }
  size = sizeof(int32_t);
  client->dfs_send(tag, size, block_buffer1);
  check_file.close();
  delete [] block_buffer1;
  delete [] block_buffer2;
}

//function for terminating the datanode process
void DataNode::terminate_all(){
  this->mtx.lock();
  this->terminate_process = true;
  this->mtx.unlock();

  for (int32_t i = 0; i < this->thread_list.size() ; i++){
    this->thread_list.at(i)->join();
  }

  for (int i = 0 ; i < this->thread_list.size(); i++){
    delete thread_list.at(i);
    thread_list.at(i) = nullptr;
  }

  this->thread_list.clear();
  this->thread_list.shrink_to_fit();
  close(this->listenfd);
  exit(1);
}
