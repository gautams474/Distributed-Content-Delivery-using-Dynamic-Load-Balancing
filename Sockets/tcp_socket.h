#ifndef TCP_SOCKET
#define TCP_SOCKET

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

class TCP_Socket{
public:
	TCP_Socket(string dest_port, string dest_address, string source_port, string source_address, bool isServer);

	TCP_Socket(string source_port, string source_address, bool isServer);

	TCP_Socket(){};

	bool server_accept(TCP_Socket& newConnection);

	bool send_to(const char* buf,const int len, int& numbytes);

	bool receiveData(char* buf, int bufLen, int& numbytes);

	void set_all_Feilds(int fd, string dest_port, string dest_address, string source_port, 
					string source_address);

	bool connect_to_destination(void);

	bool setDestination(string port, string address, bool shouldConnect);

	void disConnect(){
		isConnected = false;
	}

	void close_connection(){
		isConnected = false;
		close(sockfd);
	}

	~TCP_Socket(){
		close(sockfd);
		close(new_fd);
	}
	
private:

	int sockfd;  // listen on sock_fd, new connection on new_fd
	int new_fd;
	bool isConnected;
	bool isServer;
	static const char delim = '^';

	string dest_port;
	string dest_address;
	string my_ip;
	string my_port;

	bool setUpAddress(void);

	bool setUpSpecificAddress(string& msg, sockaddr_in& specified_addr, string& port, string& address);

	bool getAddress_n_port(string& port, string& address, struct sockaddr their_addr);

	// get sockaddr, IPv4 or IPv6:
	void* get_in_addr(struct sockaddr* sa);

	unsigned short get_in_port(struct sockaddr* sa);

	void printAddress(string msg, struct sockaddr* sa);

	bool isEqual_address(struct sockaddr_in* lhs, struct sockaddr_in* rhs);

	bool set_up_listening();
};

#endif