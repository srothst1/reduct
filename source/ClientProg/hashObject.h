
#include <string>
#include <cstdio>

using namespace std;


/*HashObject class for computing hash value of a given byte array*/
class HashObject {
public:
  HashObject(){ }
   ~HashObject(){ }
  string Sha1Hash(char *input, int32_t length);

  //TODO : implement other hash functions
  //sha2 hash function
  string Sha2Hash(char *input, int32_t length);
  //md5 hash function
  string md5Hash(char *input, int32_t length);

private:
  //converts char* to string - called by public functions
  string convertToString(unsigned char* input);
};
