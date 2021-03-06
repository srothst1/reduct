#pragma once

#include <string>
#include <cstdint>
#include <stdexcept>

using namespace std;

/*Class for connected neighbor node, which stores ip address and sockid
of the node. Provides basic protocol for sending and receiving content of
buffer of a given size with a given tag*/
class Node {
  public:
    Node(string ip_address);
    ~Node();
    int32_t getsockfd();
    string getip_address();
    int32_t dfs_send(char tag, int32_t size, const char* input);
    int32_t dfs_recv(char* tag, int32_t* size, char* output);

  private:
    string ip_address;
    int32_t sockfd;

};
