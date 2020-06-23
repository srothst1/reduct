#include "../config.h"
#include "secureMap.h"
#include "fileDirectory.h"
#include "client.h"
#include "node.h"

#include <iostream>
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


using namespace std;

MasterNode::MasterNode(){
  this->listenfd1 = 0;
  this->listenfd2 = 0;
  this->terminate_process = false;
  this->blockMAP = new SecureMap<string, int32_t>();
  this->groupMAP= new SecureMap<string, string>();
  this->filedirectory= new FileDirectory();

}

MasterNode::~MasterNode(){
  for (int i = 0 ; i < this->client_table.size(); i++){
    delete client_table.at(i);
  }
  for (int i = 0 ; i < this->client_thread_list.size(); i++){
    delete client_thread_list.at(i);
  }
  for (int i = 0 ; i < this->data_nodes.size(); i++){
    delete data_nodes.at(i);
  }
  delete this->blockMAP;
  delete this->groupMAP;
  delete this->filedirectory;
  close(this->listenfd1);
  close(this->listenfd2);
}

int32_t MasterNode::getlistenfd1(){
  return this->listenfd1;
}

int32_t MasterNode::getlistenfd2(){
  return this->listenfd2;
}


//listener thread for accpeting connection from client nodes
void MasterNode::client_listener(){
  int32_t sockfd;
  unsigned int socklen;
  struct sockaddr_in listen_addr, client_addr;
  struct linger linger_val;
  int opt = 1;
  bool terminate = this->terminate_process;

  if(!(this->listenfd1 = socket(AF_INET, SOCK_STREAM, 0))) {
    perror("Create Socket Failed\n");
    return;
  }

  //set linger values off for listenfd
  linger_val.l_onoff = 0;
  linger_val.l_linger = 0;
  if(setsockopt(this->listenfd1, SOL_SOCKET, SO_LINGER, (void *)&linger_val,
      (socklen_t) (sizeof(struct linger)))<0) {
    perror("Setting socket option failed\n");
    return;
  }

  // set SO_REUSEADDR on listenfd to true
  opt = 1;
  if (setsockopt(this->listenfd1, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))<0){
    perror("Setting socket option failed\n");
    return;
  }

  listen_addr.sin_family =  AF_INET;
  listen_addr.sin_port =  htons(PORTNUM);
  listen_addr.sin_addr.s_addr = INADDR_ANY;

  //bind the listenfd to the port
  if (bind(this->listenfd1, (struct sockaddr *)&listen_addr, sizeof(listen_addr))<0){
    perror("Binding failed\n");
    return;
  }

  //set listenfd to listen
  if(listen(this->listenfd1, 10) <0){
    perror("Binding failed\n");
    close(this->listenfd1);
    return;
  }

  socklen = sizeof(client_addr);

  //while loop leave listening port open to accept a client
  while (!terminate){
    sockfd = accept(this->listenfd1, (struct sockaddr *) &client_addr, &socklen);
    if (sockfd<0) {
      perror("TCP connnection with the client failed\n");
    }
    else{
      // cout << "new client connected \n";
      Node* new_node = new Node(sockfd);
      Client* new_client = new Client(new_node, this->filedirectory, this->blockMAP,
        this->groupMAP, &this->data_nodes, &this->global_mtx);
      thread* new_thread = new thread(&Client::handle_client_request, new_client);
      this->client_table.push_back(new_client);
      this->client_thread_list.push_back(new_thread);
    }
    this->global_mtx.lock();
    terminate = this->terminate_process;
    this->global_mtx.unlock();

  }

}

//listener thread for accepting connection from data nodes
void MasterNode::datanode_listener(){
  char tag;
  int32_t sockfd, ip_size;
  string ip;
  unsigned int socklen;
  struct sockaddr_in listen_addr, client_addr;
  struct linger linger_val;
  int opt = 1;
  bool terminate = this->terminate_process;

  if(!(this->listenfd2 = socket(AF_INET, SOCK_STREAM, 0))) {
    perror("Create Socket Failed\n");
    return;
  }

  //set linger values off for listenfd
  linger_val.l_onoff = 0;
  linger_val.l_linger = 0;
  if(setsockopt(this->listenfd2, SOL_SOCKET, SO_LINGER, (void *)&linger_val,
      (socklen_t) (sizeof(struct linger)))<0) {
    perror("Setting socket option failed\n");
    return;
  }

  // set SO_REUSEADDR on listenfd to true
  opt = 1;
  if (setsockopt(this->listenfd2, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))<0){
    perror("Setting socket option failed\n");
    return;
  }

  listen_addr.sin_family =  AF_INET;
  listen_addr.sin_port =  htons(PORTNUM2);
  listen_addr.sin_addr.s_addr = INADDR_ANY;

  //bind the listenfd to the port
  if (bind(this->listenfd2, (struct sockaddr *)&listen_addr, sizeof(listen_addr))<0){
    perror("Binding failed\n");
    return;
  }

  //set listenfd to listen
  if(listen(this->listenfd2, 10) <0){
    perror("Binding failed\n");
    close(this->listenfd2);
    return;
  }

  socklen = sizeof(client_addr);

  //while loop leave listening port open to accept a client
  while (!terminate){
    sockfd = accept(this->listenfd2, (struct sockaddr *) &client_addr, &socklen);
    if (sockfd<0) {
      perror("TCP connnection with the client failed\n");
    }
    else{
      Node* new_node = new Node(sockfd);
      if (new_node->dfs_recv(&tag, &ip_size, this->buffer)>=0){
        if (tag == IP){
          ip = this->buffer;
          new_node->setip_address(ip);
          this->data_nodes.push_back(new_node);
        }
      }

    }
    this->global_mtx.lock();
    terminate = this->terminate_process;
    this->global_mtx.unlock();

  }

}

//function for terminating the enture process
void MasterNode::terminate_all(){
  cout << "Total Size: " << this->filedirectory->gettotal_size() << endl;
  cout << "Duplicate Size: " << this->filedirectory->getduplicate_size() << endl;
  this->global_mtx.lock();
  this->terminate_process = true;
  this->global_mtx.unlock();

  for (int i = 0 ; i < this->client_table.size(); i++){
    this->client_table.at(i)->terminate_client();
  }

  for (int32_t i = 0; i < this->client_thread_list.size() ; i++){
    this->client_thread_list.at(i)->join();
  }

  for (int i = 0 ; i < this->client_table.size(); i++){
    delete client_table.at(i);
    this->client_table.at(i) = nullptr;
  }
  for (int i = 0 ; i < this->client_thread_list.size(); i++){
    delete client_thread_list.at(i);
    this->client_thread_list.at(i) = nullptr;
  }
  for (int i = 0 ; i < this->data_nodes.size(); i++){
    delete data_nodes.at(i);
    this->data_nodes.at(i) = nullptr;
  }
  client_table.clear();
  client_thread_list.clear();
  data_nodes.clear();
  client_table.shrink_to_fit();
  client_thread_list.shrink_to_fit();
  data_nodes.shrink_to_fit();
  delete this->blockMAP;
  delete this->groupMAP;
  delete this->filedirectory;
  this->blockMAP = nullptr;
  this->groupMAP = nullptr;
  this->filedirectory = nullptr;
  exit(1);
}
