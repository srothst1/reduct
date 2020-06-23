#pragma once

#include <string>

using namespace std;

//public class for file system inode
class Inode {
  public:
    virtual ~Inode(){   };
    virtual bool isdirectory() = 0;
};
