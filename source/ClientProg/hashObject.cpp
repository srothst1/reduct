#include "hashObject.h"

#include <iostream>
#include <string>
#include <cstdio>
#include <openssl/sha.h>
#include <sstream>

using namespace std;

//Sha1 hash function
string HashObject::Sha1Hash(char *input, int32_t length){
  //TODO: convert the string into a char string[] type + keep a variable with the length
  const int input_length = length;

  unsigned char digest[SHA_DIGEST_LENGTH];
  SHA_CTX context;
  SHA1_Init(&context);
  SHA1_Update(&context, (unsigned char*) input, input_length);
  SHA1_Final(digest, &context);

  return this->convertToString(digest);
}


//TODO : implement other hash functions
//sha2 hash function
string HashObject::Sha2Hash(char *input, int32_t length){
  return "";
}
//md5 hash function
string HashObject::md5Hash(char *input, int32_t length){
  return "";
}

//Converts char array to a hexadecimal string
string HashObject::convertToString(unsigned char* input){
  stringstream ss;
  for(int i=0; i<SHA_DIGEST_LENGTH; ++i){
    ss << hex << (int32_t) input[i];
  }

  string output = ss.str();
  return output;
}
