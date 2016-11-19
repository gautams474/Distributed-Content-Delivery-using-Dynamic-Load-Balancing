#include <sys/stat.h>
#include <unistd.h>
#include <string>

using namespace std;

inline bool exists_test3 (const string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0  && S_ISDIR(buffer.st_mode)); 
}


int main(int argc, char** argv){

	

	return 0;
}