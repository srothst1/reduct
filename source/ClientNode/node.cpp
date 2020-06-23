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

Node::Node(string ip_address){
  this->ip_address = ip_address;
  struct sockaddr_in node_addr;
  if ((this->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
      throw invalid_argument("Socket creation failed");
  }
  node_addr.sin_port =  htons(PORTNUM);
  node_addr.sin_family =  AF_INET;
  if(inet_pton(AF_INET, this->ip_address.c_str(), &node_addr.sin_addr)<=0){
      throw invalid_argument("Invalid address/ Address not supported");
  }
  if (connect(this->sockfd, (struct sockaddr *)&node_addr, sizeof(node_addr)) < 0){
      throw invalid_argument("Invalid address/ Address not supported");
  }

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

//sends the message tag, size, and content in order.
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

//receives the message tag, size, and content in order.
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
