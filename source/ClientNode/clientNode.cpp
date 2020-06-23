#include "clientNode.h"
#include "../config.h"

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

ClientNode::ClientNode(string master_node_ip){
  this->master_node_ip = master_node_ip;
  this->listenfd = 0;
  this->terminate_process = false;
  this->master_node = nullptr;

}

ClientNode::~ClientNode(){
  for (int i = 0 ; i < this->client_table.size(); i++){
    delete client_table.at(i);
  }
  for (int i = 0 ; i < this->thread_list.size(); i++){
    delete thread_list.at(i);
  }
  delete this->master_node;

  close(this->listenfd);
}

int32_t ClientNode::getlistenfd(){
  return this->listenfd;
}

string ClientNode::getmaster_node_ip(){
  return this->master_node_ip;
}

//listner thread for accepting new TCP connection from a client program
void ClientNode::listener(){
  int32_t sockfd;
  unsigned int socklen;
  struct sockaddr_in listen_addr, client_addr;
  struct linger linger_val;
  int opt = 1;
  bool terminate = this->terminate_process;
  try{
    this->master_node = new Node(this->master_node_ip);

  }
  catch (exception& e){
    cout << "Invalid ip\n";
    return;
  }


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
  listen_addr.sin_port =  htons(PORTNUM2);
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
      Client* new_client = new Client((int32_t) this->client_table.size(), sockfd, this->master_node,
      &this->global_mtx1, &this->global_mtx2, &this->terminate_process);
      thread* new_thread = new thread(&Client::handle_client_request, new_client);
      this->client_table.push_back(new_client);
      this->thread_list.push_back(new_thread);
    }
    this->global_mtx1.lock();
    terminate = this->terminate_process;
    this->global_mtx1.unlock();

  }

}

//terminates all threads and deallocate all data structures, then terminates the process.
void ClientNode::terminate_all(){
  this->global_mtx1.lock();
  this->terminate_process = true;
  this->global_mtx1.unlock();
  for (int i = 0 ; i < this->client_table.size(); i++){
    client_table.at(i)->terminate_client();
  }

  for (int32_t i = 0; i < this->thread_list.size() ; i++){
    this->thread_list.at(i)->join();
  }

  for (int i = 0 ; i < this->client_table.size(); i++){
    delete client_table.at(i);
    client_table.at(i) = nullptr;
  }
  for (int i = 0 ; i < this->thread_list.size(); i++){
    delete thread_list.at(i);
    thread_list.at(i) = nullptr;
  }
  this->client_table.clear();
  this->thread_list.clear();
  this->client_table.shrink_to_fit();
  this->thread_list.shrink_to_fit();
  delete this->master_node;
  this->master_node = nullptr;
  close(this->listenfd);
  exit(1);

}
