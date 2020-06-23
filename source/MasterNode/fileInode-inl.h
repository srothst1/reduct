#include "secureMap.h"
#include "fileInode.h"
#include <vector>
#include <string>
#include <cstdint>

using namespace std;

FileInode::FileInode(){
  this->isdir = false;
}

FileInode::~FileInode(){

}

int32_t FileInode::size(){
    return (int32_t) this->inode_table.size();
}
string FileInode::getblockID(int32_t offset){
  if (offset<this->inode_table.size()){
    return this->inode_table.at(offset);
  }
  return "";
}
bool FileInode::setblockID(int32_t offset, string blockID){
  if (offset<this->inode_table.size()){
    this->inode_table.at(offset) = blockID;
    return true;
  }
  else if(offset == this->inode_table.size()){
    this->inode_table.push_back(blockID);
    return true;
  }
  return false;
}

bool FileInode::isdirectory(){
  return this->isdir;
}
