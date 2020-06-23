#include "../config.h"
#include "node.h"
#include <string>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>


using namespace std;

Node::Node(int32_t sockfd){
  this->sockfd = sockfd;
  this->ip_address = "";
}

Node::~Node(){
  close(sockfd);
}

int32_t Node::getsockfd(){
  return this->sockfd;
}

string Node::getip_address(){
  return this->ip_address;
}
void Node::setip_address(string new_ip){
  this->ip_address = new_ip;
}

//function for sending a message to the node
int32_t Node::dfs_send(char tag, int32_t size, const char* input){
  if (send(this->sockfd, &tag, sizeof(char), MSG_NOSIGNAL) <1){
    return -1;
  }
  if (send(this->sockfd, &size, sizeof(int32_t), MSG_NOSIGNAL) <1){
    return -1;
  }
  if (send(this->sockfd, input, sizeof(char)*size, MSG_NOSIGNAL) <1){
    return -1;
  }
  return 0;

}

//function for receiving a message from the node
int32_t Node::dfs_recv(char* tag, int32_t* size, char* output){
  if (recv(this->sockfd, tag, sizeof(char), MSG_WAITALL) <1){
    return -1;
  }
  if (recv(this->sockfd, size, sizeof(int32_t), MSG_WAITALL) <1){
    return -1;
  }
  if (recv(this->sockfd, output, sizeof(char)* (*size), MSG_WAITALL) <1){
    return -1;
  }
  return 0;
}
