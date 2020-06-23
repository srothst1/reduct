#include "../ClientProg/fileObject.h"

#include <iostream>
#include <string>
#include <cstdio>
#include <chrono>
#include <random>

using namespace std;


int main(int argc, char** argv){
  string ip = argv[1];
  FileObject* new_object = new FileObject(ip);
  int32_t filehandler;
  char* foo = new char[1048576];
  random_device rd;
  mt19937 mt(rd());
  uniform_int_distribution<int> dst(0,127);
  filehandler= new_object->dfs_open("foo2", "w");
  auto start = chrono::steady_clock::now();
  for(int j = 0; j < 256; j++){
    for (int i =0; i < 1048576; i++){
      foo[i]=dst(mt);
    }
    new_object->dfs_write(filehandler, foo, 1048576);
  }
  auto end = chrono::steady_clock::now();

  cout << "foo2 write time in milliseconds : "
		<< chrono::duration_cast<chrono::milliseconds>(end - start).count()
		<< " ms" << endl;

  new_object->dfs_close(filehandler);

  filehandler= new_object->dfs_open("foo2", "r");

  start = chrono::steady_clock::now();
  for (int j = 0; j < 256; j++){
    new_object->dfs_read(filehandler, foo, 1048576);
  }
  end = chrono::steady_clock::now();

  cout << "foo2 read time in milliseconds : "
		<< chrono::duration_cast<chrono::milliseconds>(end - start).count()
		<< " ms" << endl;

  new_object->dfs_close(filehandler);
  delete new_object;
  delete[] foo;
  return 0;
}
