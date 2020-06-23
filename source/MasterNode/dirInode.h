#pragma once

#include "secureMap.h"
#include "inode.h"
#include <vector>
#include <string>

using namespace std;

//class for directory inodes
class DirInode : public Inode {
  public:
    DirInode();
    ~DirInode();

    vector<string> getchildren();
    Inode* getnext(string key);
    bool contains(string key);
    void insert(string key, Inode* new_inode);
    void update(string key, Inode* new_inode);
    void remove(string key);
    bool isdirectory();


  private:
    SecureMap<string, Inode*> children;
    bool isdir;

};


#include "dirInode-inl.h"
