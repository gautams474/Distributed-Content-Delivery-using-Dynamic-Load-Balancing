#ifndef UDP_SOCKET
#define UDP_SOCKET

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

class UDP_Socket{
public:
	UDP_Socket(string dest_port, string dest_address, string source_port,   
					string source_address, bool isServer);  //1

	UDP_Socket(string source_port, string source_address, bool isServer);

	bool connect_to_destination(void);

	bool setDestination(string port, string address, bool shouldConnect);

	bool send_to(char* buf, int len, int& numbytes); //1

	bool send_to(char* buf, int len, int& numbytes, string& dest_port, string dest_address);

	bool start_receiving(char* buf, int bufLen, int& numbytes, string& dest_port, string& dest_address);

	void disConnect(){
		isConnected = false;
	}

	void close_connection(){
		isConnected = false;
		close(sockfd);
	}

	~UDP_Socket(){
		dest_port="";
		dest_address="";
		my_ip="";
		my_port="";

		isConnected = false;
		isServer = false;
		close(sockfd);
	}
	
private:

	int sockfd;  // listen on sock_fd, new connection on new_fd
	bool isConnected;
	bool isServer;

	static const char delim = '^';
	string dest_port;
	string dest_address;
	string my_ip;
	string my_port;

	bool setUpAddress(void);

	bool setUpSpecificAddress(string& msg, sockaddr_in& specified_addr, string& port, string& address);

	bool getAddress_n_port(string& port, string& address, struct sockaddr_storage their_addr);

	// get sockaddr, IPv4 or IPv6:
	void* get_in_addr(struct sockaddr* sa);

	unsigned short get_in_port(struct sockaddr* sa);

	void printAddress(string msg, struct sockaddr* sa);

	bool isEqual_address(struct sockaddr_in* lhs, struct sockaddr_in* rhs);

};

#endif