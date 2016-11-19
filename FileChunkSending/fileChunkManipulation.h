#include "./../Sockets/tcp_socket.h"

using namespace std;

class Server:TCP_Socket{

private:

	string FileName;
	int baseChunk;
	int numberofFiles;

	static string baseAddress = "/home/prameet/ACN-Project/Content/"
	static string subscript= "_dir";
};