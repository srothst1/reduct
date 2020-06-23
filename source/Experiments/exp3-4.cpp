
#include "../ClientProg/fileObject.h"

#include <iostream>
#include <string>
#include <cstdio>
#include <chrono>

using namespace std;


int main(int argc, char** argv){
  string ip = argv[1];
  FileObject* new_object = new FileObject(ip);
  int32_t filehandler;
  char* foo = new char[1048576];
  int counter = 0;
  auto start = chrono::steady_clock::now();

  filehandler= new_object->dfs_open("foo4-1", "w");
  for(int j = 0; j < 151; j++){
    for (int i =0; i < 1048576; i++){
      foo[i]= counter;
      counter = (counter+1) % 151;
    }
    new_object->dfs_write(filehandler, foo, 1048576);
  }
  new_object->dfs_close(filehandler);

  counter =0;
  filehandler= new_object->dfs_open("foo4-2", "w");
  for(int j = 0; j < 43; j++){
    for (int i =0; i < 1048576; i++){
      foo[i]= counter;
      counter = (counter+1) % 43;
    }
    new_object->dfs_write(filehandler, foo, 1048576);
  }
  new_object->dfs_close(filehandler);

  counter =0;
  filehandler= new_object->dfs_open("foo4-3", "w");
  for(int j = 0; j < 19; j++){
    for (int i =0; i < 1048576; i++){
      foo[i]= counter;
      counter = (counter+1) % 19;
    }
    new_object->dfs_write(filehandler, foo, 1048576);
  }
  new_object->dfs_close(filehandler);

  counter =0;
  filehandler= new_object->dfs_open("foo4-4", "w");
  for(int j = 0; j < 37; j++){
    for (int i =0; i < 1048576; i++){
      foo[i]= counter;
      counter = (counter+1) % 37;
    }
    new_object->dfs_write(filehandler, foo, 1048576);
  }
  new_object->dfs_close(filehandler);

  auto end = chrono::steady_clock::now();

  cout << "foo write time in milliseconds : "
		<< chrono::duration_cast<chrono::milliseconds>(end - start).count()
		<< " ms" << endl;


  start = chrono::steady_clock::now();

  filehandler= new_object->dfs_open("foo4-1", "r");
  for (int j = 0; j < 151; j++){
    new_object->dfs_read(filehandler, foo, 1048576);
  }

  filehandler= new_object->dfs_open("foo4-2", "r");
  for (int j = 0; j < 43; j++){
    new_object->dfs_read(filehandler, foo, 1048576);
  }

  filehandler= new_object->dfs_open("foo4-3", "r");
  for (int j = 0; j < 19; j++){
    new_object->dfs_read(filehandler, foo, 1048576);
  }

  filehandler= new_object->dfs_open("foo4-4", "r");
  for (int j = 0; j < 37; j++){
    new_object->dfs_read(filehandler, foo, 1048576);
  }

  end = chrono::steady_clock::now();

  cout << "foo read time in milliseconds : "
		<< chrono::duration_cast<chrono::milliseconds>(end - start).count()
		<< " ms" << endl;

  new_object->dfs_close(filehandler);
  delete new_object;
  delete[] foo;
  return 0;
}
