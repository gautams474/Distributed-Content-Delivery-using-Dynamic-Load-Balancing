#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdexcept>
#include <iostream>
#include <sstream>

#include <Sockets/udp_socket.h>

using namespace std;

UDP_Socket::UDP_Socket(string dest_port, string dest_address, string source_port, 
					string source_address, bool isServer){

		this->dest_port = dest_port;
		this->dest_address = dest_address;
		this->my_ip = source_address;
		this->my_port = source_port;

		this->isServer = isServer;

		if(setUpAddress() == false){
			cout << "setting up address failed " << endl;
		}

		connect_to_destination();
}

UDP_Socket::UDP_Socket(string source_port, string source_address, bool isServer){
		this->my_ip = source_address;
		this->my_port = source_port;

		this->isServer = isServer;

		if(setUpAddress() == false){
			cout << "setting up address failed " << endl;
		}
	}

	bool UDP_Socket::connect_to_destination(void){
		if(dest_address.length() == 0 && dest_port.length() == 0){
			cerr << "destination address or port not specified" << endl;
			return false;
		}

		struct sockaddr_in specified_addr;
		memset(&specified_addr, 0, sizeof(specified_addr));

		cout << "my port " << dest_port << " my ip " << dest_address << endl;
		int i_my_port;
		int ret;
		specified_addr.sin_family = AF_INET;
		sscanf(dest_port.c_str(), "%d", &i_my_port);
		specified_addr.sin_port = htons(i_my_port);
		if((ret = inet_pton(AF_INET, dest_address.c_str(), &(specified_addr.sin_addr))) <= 0){  // IPv4
			if(ret == 0){
				cerr << "incorrect IP address" << endl;
				throw runtime_error("incorrect IP address" );
			}
			else{
				perror("pton: IP conversion failed: ");
				throw runtime_error("pton: IP conversion failed: ");
			}
			return false;
		}

		connect(sockfd, (const sockaddr*) (&specified_addr), sizeof(specified_addr));
		isConnected = true;
		return true;
	}

	bool UDP_Socket::setDestination(string port, string address, bool shouldConnect){
		this->dest_port = port;
		this->dest_address = address;

		if(shouldConnect)
			return connect_to_destination();

		return true;
	}

	bool UDP_Socket::send_to(char* buf, int len, int& numbytes){
		if(!isConnected){
			cerr << "dest_address and port is not connected to"<< endl;
			return false;
		}
		
		if((numbytes = send(sockfd, buf, len, 0 )) == -1){
				perror("talker: sendto");
				return false;
		}
		return true;
		
	}

	bool UDP_Socket::send_to(char* buf, int len, int& numbytes, const string& dest_port, const string dest_address){

		struct sockaddr_in specified_addr;
		memset(&specified_addr, 0, sizeof(specified_addr));

		if(dest_port.length() > 0 && dest_address.length() > 0){
			cout << "destination port " << my_port << " destination ip " << my_ip << endl;
			int i_my_port;
			int ret;
			specified_addr.sin_family = AF_INET;
			sscanf(dest_port.c_str(), "%d", &i_my_port);
			specified_addr.sin_port = htons(i_my_port);

			if((ret = inet_pton(AF_INET, dest_address.c_str(), &(specified_addr.sin_addr))) <= 0){  // IPv4
				if(ret == 0){
					cerr << "incorrect IP address" << endl;
					throw runtime_error("incorrect IP address" );
				}
				else{
					perror("pton: IP conversion failed: ");
					throw runtime_error("pton: IP conversion failed: ");
				}
				return false;
			}
		}

		// TO DO: Check this
		struct sockaddr* send_addr = (struct sockaddr *)&specified_addr;

		printAddress("send to address: ", send_addr);
		if((numbytes = sendto(sockfd, buf, len, 0, send_addr, sizeof(*send_addr))) == -1){
			perror("talker: sendto");
			return false;
		}
		return true;
		
	}

	bool UDP_Socket::start_receiving(char* buf, int bufLen, int& numbytes, string& dest_port, string& dest_address){
		struct sockaddr_storage their_addr; // connector's address information
		socklen_t addr_len;

		memset(&their_addr, 0, sizeof(their_addr));
		printf("listener: waiting to recvfrom...\n");

		addr_len = sizeof(their_addr);
		int recvdBytes = 0;
		while(recvdBytes != bufLen){
			if((numbytes = recvfrom(sockfd, &(buf[recvdBytes]), bufLen-recvdBytes,0 , (struct sockaddr *)&their_addr, &addr_len)) == -1){
				perror("recvfrom");
				return false;
			}
			recvdBytes += numbytes;
			if(buf[recvdBytes] == delim)
				break;
		}
		numbytes = recvdBytes;
		return getAddress_n_port(dest_port, dest_address, their_addr);
	}

	bool UDP_Socket::setUpSpecificAddress(string& msg, sockaddr_in& specified_addr, string& port, string& address){
		memset(&specified_addr, 0, sizeof(specified_addr));

		if(my_port.length() > 0 && my_ip.length() > 0){
			int i_my_port;
			int ret;

			cout << msg  << my_ip  << " " << my_port << endl;

			specified_addr.sin_family = AF_INET;

			sscanf(my_port.c_str(), "%d", &i_my_port);
			specified_addr.sin_port = htons(i_my_port);

			if((ret = inet_pton(AF_INET, my_ip.c_str(), &(specified_addr.sin_addr))) <= 0){  // IPv4
				if(ret == 0){
					cerr << "incorrect IP address" << endl;
					throw runtime_error("incorrect IP address" );
				}
				else{
					perror("pton: IP conversion failed: ");
					throw runtime_error("pton: IP conversion failed: ");
				}
				return false;
			}
		}
		return true;
	}

	bool UDP_Socket::setUpAddress(void){
		struct addrinfo hints, *servinfo;
		struct addrinfo* p;
		int rv;

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_flags = AI_V4MAPPED;
		hints.ai_protocol = IPPROTO_UDP;
		
		struct sockaddr_in specified_addr;
		string msg("checking for ");
		setUpSpecificAddress(msg, specified_addr, my_port, my_ip);

		const int yes=1;
		if(isServer){
			if((rv = getaddrinfo(my_ip.c_str(), my_port.c_str(), &hints, &servinfo)) != 0){
				cerr << "getaddrinfo: " << gai_strerror(rv) << endl;
				return false;
			}
		}
		else{
			if(dest_address.length() == 0){
				throw runtime_error("destination address not specified");
				return false;
			}
			if(dest_port.length() == 0){
				throw runtime_error("destination port not specified");
				return false;
			}
			if((rv = getaddrinfo(my_ip.c_str(), my_port.c_str(), &hints, &servinfo)) != 0){
				cerr << "getaddrinfo: " << gai_strerror(rv) << endl;
				return false;
			}
		}
		// struct sockaddr* ptr_specified_addr = (struct sockaddr*)&specified_addr;
		// loop through all the results and bind to the first we can
		for(p = servinfo; p != NULL; p = p->ai_next){
			if(!isEqual_address((struct sockaddr_in*)(p->ai_addr), &specified_addr) && specified_addr.sin_family != 0)
				continue;

		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		/*to remove the cannot bind to address*/
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
			perror("setsockopt");
			exit(1);
		}
		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			perror("server: bind");
			continue;
		}
			break;
		}

		if (p == NULL){
			freeaddrinfo(servinfo); // all done with this structure
			cerr << "server: failed to bind" << endl;
			throw runtime_error("server: failed to bind");
			return false;
		}

		printAddress("Bound to", p->ai_addr);

		freeaddrinfo(servinfo); // all done with this structure

		return true;
	}

	bool UDP_Socket::getAddress_n_port(string& port, string& address, struct sockaddr_storage their_addr){
		char addr_buf[INET_ADDRSTRLEN];

		unsigned short short_port = ntohs(get_in_port((struct sockaddr*)  &their_addr));
		stringstream stream_port;  stream_port << short_port;
		port = stream_port.str();

		struct in_addr* addr = (struct in_addr*)get_in_addr((struct sockaddr*)  &their_addr);
		if(inet_ntop(AF_INET, addr, addr_buf, INET_ADDRSTRLEN) == NULL){
			perror("Could not print IP address: ");
			throw runtime_error("Could not print IP address");
			return false;
		}
		string s_addr(addr_buf);
		address = s_addr;
		return true;
	}

	// get sockaddr, IPv4 or IPv6:
	void* UDP_Socket::get_in_addr(struct sockaddr* sa){
		if (sa->sa_family == AF_INET){
			return &(((struct sockaddr_in*)sa)->sin_addr);
		}
		return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}

	// get sockaddr, IPv4 or IPv6:
	// get sockaddr, IPv4 or IPv6:
	unsigned short UDP_Socket::get_in_port(struct sockaddr* sa){
		if (sa->sa_family == AF_INET) {
			return ((((struct sockaddr_in*)sa)->sin_port));
		}

		return ((((struct sockaddr_in6*)sa)->sin6_port));
	}

	void UDP_Socket::printAddress(string msg, struct sockaddr* sa){
		char addr_buf[INET_ADDRSTRLEN];
		struct in_addr* addr = (struct in_addr*)get_in_addr(sa);
		unsigned short port = ntohs(get_in_port(sa));
		if(inet_ntop(AF_INET, addr, addr_buf, INET_ADDRSTRLEN) == NULL){
			perror("Could not print IP address: ");
			throw runtime_error("Could not print IP address");
			return;
		}
		cout << msg << " " << addr_buf << " " << port << endl;
	}

	bool UDP_Socket::isEqual_address(struct sockaddr_in* lhs, struct sockaddr_in* rhs){
		printAddress("checking if equal", (struct sockaddr*) lhs);
		printAddress("to", (struct sockaddr*) rhs);
		if(lhs->sin_family != rhs->sin_family){
			cout << "sin_family !=" << endl;
			return false;
		}
		if(lhs->sin_port != rhs->sin_port){
			cout << "sin_port !=" << endl;
			return false;
		}
		if((lhs->sin_addr).s_addr != (rhs->sin_addr).s_addr){
			cout << "sin_addr !=" << endl;
			return false;
		}

		// cout << "address found " << endl;
		return true;
	}

/*
int main(void){

	string port = "10001";
	string address = "10.0.3.15";

	UDP_Socket server1(port, address, true);

	int bufLen = 1025;
	char buf[bufLen];
	memset(buf,0,bufLen);

	int numbytes;
	string dest_port;
	string dest_address;

	server1.start_receiving(buf, bufLen-1, numbytes, dest_port, dest_address);

	cout << "Received: " << buf << endl;

    return 0;
}*/