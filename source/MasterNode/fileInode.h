#pragma once

#include "secureMap.h"
#include "inode.h"
#include <vector>
#include <string>
#include <cstdint>

using namespace std;

//Class for file inode
class FileInode : public Inode {
  public:
    FileInode();
    ~FileInode();
    int32_t size();
    string getblockID(int32_t offset);
    bool setblockID(int32_t offset, string blockID);
    bool isdirectory();

  private:
    vector<string> inode_table;
    bool isdir;

};

#include "fileInode-inl.h"
