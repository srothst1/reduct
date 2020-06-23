
#include "inode.h"
#include "dirInode.h"
#include "fileInode.h"
#include "fileDirectory.h"
#include <vector>
#include <string>
#include <cstdint>
#include <mutex>
#include <iostream>

using namespace std;

FileDirectory::FileDirectory(){
  this->total_size = 0;
  this->duplicate_size = 0;
  this->root = new DirInode();
}

FileDirectory::~FileDirectory(){
  delete this->root;
}

long FileDirectory::gettotal_size(){
  this->mtx.lock();
  int32_t size = this->total_size;
  this->mtx.unlock();
  return size;
}
long FileDirectory::getduplicate_size(){
  this->mtx.lock();
  int32_t size = this->duplicate_size;
  this->mtx.unlock();
  return size;
}

void FileDirectory::settotal_size(long size){
  this->mtx.lock();
  this->total_size = size;
  this->mtx.unlock();
}

void FileDirectory::setduplicate_size(long size){
  this->mtx.lock();
  this->duplicate_size = size;
  this->mtx.unlock();
}


bool FileDirectory::mkdir(vector<string> dir_name){
  this->mtx.lock();
  DirInode* current_node = this->root;
  int32_t counter = 0;
  while ((counter < dir_name.size()-1)) {
    if (current_node->contains(dir_name.at(counter)) && current_node->getnext(dir_name.at(counter))->isdirectory()){
      current_node = (DirInode*) current_node->getnext(dir_name.at(counter));
      counter ++;
    }
    else {
      this->mtx.unlock();
      return false;
    }
  }
  if (!current_node->contains(dir_name.at(counter))){
    Inode* new_dir= new DirInode();
    current_node->insert(dir_name.at(counter), new_dir);
  }
  this->mtx.unlock();
  return true;
}

bool FileDirectory::open(vector<string> dir_name, string mode){
  this->mtx.lock();
  DirInode* current_node = this->root;
  int32_t counter = 0;
  while ((counter < dir_name.size()-1)) {
    if (current_node->contains(dir_name.at(counter)) && (current_node->getnext(dir_name.at(counter)))->isdirectory()){
      current_node = (DirInode*) current_node->getnext(dir_name.at(counter));
      counter ++;
    }
    else {
      this->mtx.unlock();
      return false;
    }
  }
  // cout <<"trying to open" << dir_name.at(counter) <<endl;
  if (!current_node->contains(dir_name.at(counter)) && mode =="w"){
    Inode* new_dir= new FileInode();
    current_node->insert(dir_name.at(counter), new_dir);
    this->mtx.unlock();
    // cout <<"successfully opened " << dir_name.at(counter) <<endl;
    return true;
  }
  else if (current_node->contains(dir_name.at(counter)) && mode =="r"){
    this->mtx.unlock();
    // cout <<"successfully opened " << dir_name.at(counter) <<endl;
    return true;
  }
  this->mtx.unlock();
  return false;
}

string FileDirectory::getblockID(vector<string> dir_name, int32_t offset){
  this->mtx.lock();
  DirInode* current_node = this->root;
  int32_t counter = 0;
  string blockID;
  while ((counter < dir_name.size()-1)) {
    if (current_node->contains(dir_name.at(counter)) && (current_node->getnext(dir_name.at(counter)))->isdirectory()){
      current_node = (DirInode*) current_node->getnext(dir_name.at(counter));
      counter ++;
    }
    else {
      this->mtx.unlock();
      return "";
    }
  }
  if (current_node->contains(dir_name.at(counter)) && !(current_node->getnext(dir_name.at(counter)))->isdirectory()){
    FileInode* file_inode = (FileInode*) current_node->getnext(dir_name.at(counter));
    blockID = file_inode->getblockID(offset);
    this->mtx.unlock();
    return blockID;
  }
  this->mtx.unlock();
  return "";
}


bool FileDirectory::setblockID(vector<string> dir_name, int32_t offset, string blockID){
  this->mtx.lock();
  DirInode* current_node = this->root;
  int32_t counter = 0;
  bool success;
  while ((counter < dir_name.size()-1)) {
    if (current_node->contains(dir_name.at(counter)) && (current_node->getnext(dir_name.at(counter)))->isdirectory()){
      current_node = (DirInode*) current_node->getnext(dir_name.at(counter));
      counter ++;
    }
    else {
      this->mtx.unlock();
      return false;
    }
  }
  if (current_node->contains(dir_name.at(counter)) && !(current_node->getnext(dir_name.at(counter)))->isdirectory()){
    FileInode* file_inode = (FileInode*) current_node->getnext(dir_name.at(counter));
    success = file_inode->setblockID(offset, blockID);
    this->mtx.unlock();
    return success;
  }
  this->mtx.unlock();
  return false;
}
