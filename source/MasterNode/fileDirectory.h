#pragma once

#include "inode.h"
#include "dirInode.h"
#include "fileInode.h"
#include <vector>
#include <string>
#include <cstdint>
#include <mutex>

using namespace std;

/*FileDireoctory class for storing file directory information
consists of tree of file and directory inodes*/
class FileDirectory {
  public:
    FileDirectory();
    ~FileDirectory();
    long gettotal_size();
    long getduplicate_size();
    void settotal_size(long size);
    void setduplicate_size(long size);
    bool mkdir(vector<string> dir_name);
    bool open(vector<string> dir_name, string mode);
    string getblockID(vector<string> dir_name, int32_t offset);
    bool setblockID(vector<string> dir_name, int32_t offset, string blockID);

  private:
    long total_size;
    long duplicate_size;
    DirInode* root;
    mutex mtx;

};

#include "fileDirectory-inl.h"
