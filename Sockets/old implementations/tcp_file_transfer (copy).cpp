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
//#define PORT "3490"  // the port users will be connecting to

#define BACKLOG 10     // how many pending connections queue will hold
using namespace std;
void sigchld_handler(int s){
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

class UDP_Server{
public:
	UDP_Server(string port, string address){
		this->port = port;
		this->address = address;

		if(setUpAddress() == false){
			cout << "setting up address failed " << endl;
		}

		isUnassigned = false;
	}

	UDP_Server(){
		isUnassigned = true;
	}

	void send_to(char* buf, int len, int& numbytes){
		if(!isConnected){
			if((numbytes = send(sockfd, buf, len, 0 )) == -1){
				perror("talker: sendto");
				exit(1);
			}
			return;
		}
		else{
			/*TO DO: Check this*/
			struct addrinfo send_addr = &their_addr;
			if((numbytes = sendto(sockfd, buf, len, 0, send_addr->ai_addr, send_addr->ai_addrlen)) == -1){
				perror("talker: sendto");
				exit(1);
			}
			return;
		}
	}

	bool accept(char* buf, int bufLen, int& numbytes){
		printf("listener: waiting to recvfrom...\n");

	    addr_len = sizeof(their_addr);
	    if((numbytes = recvfrom(sockfd, buf, bufLen , 0, (struct sockaddr *)&their_addr, &addr_len)) == -1){
	        perror("recvfrom");
	        exit(1);
	    }
	}

	int connect(struct sockaddr_storage their_addr){
		struct addrinfo res = (struct addrinfo hints) their_addr;
		if(connect(sockfd, res.ai_addr, res.ai_addrlen) < 0){
			perror();
			throw runtime_error("connect failed");
		}
		isConnected = true;
	}

	void disConnect(){
		isConnected = false;
	}

	void close_connection(){
		isConnected = false;
		close(sockfd);
	}

	UDP_Server& operator=(UDP_Server& rhs){

	}
	
private:

	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    bool isConnected;
    socklen_t sin_size;
    struct sigaction sa;

    socklen_t addr_len;

    char s[INET6_ADDRSTRLEN];
    int rv;

    string dest_port;
    string dest_address;
    string my_ip;
    string my_port;

    bool isUnassigned;

    void copy_data(UDP_Server& rhs){
    	if(this == &rhs){
    		return;
    	}
    	if(rhs.isUnassigned ==  false){
    		rhs.close_connection();
    	}

    	rhs.port = this->port;
		rhs.address = this->address;

		rhs.setUpAddress();
    }

    bool setUpAddress(){
    	memset(&hints, 0, sizeof hints);
	    hints.ai_family = AF_INET;
	    hints.ai_socktype = SOCK_DGRAM;
	    hints.ai_flags = AI_PASSIVE; // use my IP

	    const int yes=1;

	    if((rv = getaddrinfo(NULL, dest_port.c_str(), &hints, &servinfo)) != 0){
	        cerr << "getaddrinfo: " << gai_strerror(rv) << endl;
	        return false;
	    }

		// loop through all the results and bind to the first we can
	    for(p = servinfo; p != NULL; p = p->ai_next){

	    	
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

	    if (p == NULL)  {
	        cerr << "server: failed to bind" << endl;
	        throw runtime_error("server: failed to bind");
	    }

	    freeaddrinfo(servinfo); // all done with this structure

	    return true;
    }

	    // get sockaddr, IPv4 or IPv6:
	void* get_in_addr(struct sockaddr *sa){
	    if (sa->sa_family == AF_INET) {
	        return &(((struct sockaddr_in*)sa)->sin_addr);
	    }

	    return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}


	bool set_up_sigHandler(){
		sa.sa_handler = sigchld_handler; // reap all dead processes
	    sigemptyset(&sa.sa_mask);
	    sa.sa_flags = SA_RESTART;
	    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
	        perror("sigaction");
	        throw runtime_error("sigaction not set"); 
	        return false;
	    }
	    return true;
	}

};

int main(void){

	string port = "10000";
	string address = "10.0.3.15";

	int bufflen = 1024;
	char buf[bufflen];


	UDP_Server server1(port, address);
    return 0;
}