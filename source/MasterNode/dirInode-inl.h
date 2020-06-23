
#include "secureMap.h"
#include "dirInode.h"
#include <vector>
#include <string>


using namespace std;


DirInode::DirInode(){
  this->isdir = true;
}

DirInode::~DirInode(){
  vector<string> inodes = this->children.getkeys();
  for (int i = 0; i < inodes.size(); i++){
    delete this->children.get(inodes.at(i));
  }
}

vector<string> DirInode::getchildren(){
  return this->children.getkeys();
}

Inode* DirInode::getnext(string key){
  return this->children.get(key);
}

bool DirInode::contains(string key){
  return this->children.contains(key);
}

void DirInode::insert(string key, Inode* new_inode){
  this->children.insert(key, new_inode);
}

void DirInode::update(string key, Inode* new_inode){
  this->children.update(key, new_inode);
}

void DirInode::remove(string key){
  this->children.remove(key);
}

bool DirInode::isdirectory(){
  return this->isdir;
}
