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

class TCP_Server{
public:
	TCP_Server(string port, string address){
		this->port = port;
		this->address = address;

		if(setUpAddress() == false){
			cout << "setting up address failed " << endl;
		}
		if(set_up_listening() == false){
			cout << "setting up listening failed " << endl;
		}

		if(set_up_sigHandler() == false){
			cout << "setting up sig child hanlder failed " << endl;
		}
	}
	
	void server_accept(){
		cout << "server: waiting for connections...\n" << endl;

	    while(1) {  // main accept() loop
	        sin_size = sizeof their_addr;
	        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	        if (new_fd == -1) {
	            perror("accept");
	            continue;
	        }

	        inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),s, sizeof(s));
	        printf("server: got connection from %s\n", s);

	        if (!fork()) { // this is the child process
	            close(sockfd); // child doesn't need the listener
	            if (send(new_fd, "Hello, world!", 13, 0) == -1)
	                perror("send");
	            close(new_fd);
	            exit(0);
	        }
	        close(new_fd);  // parent doesn't need this
	    }
	}

	void connect(){
		connect(sockfd, res->ai_addr, res->ai_addrlen);
	}

private:

	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;

    char s[INET6_ADDRSTRLEN];
    int rv;

    string port;
    string address;

    bool setUpAddress(){
    	memset(&hints, 0, sizeof hints);
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_STREAM;
	    hints.ai_flags = AI_PASSIVE; // use my IP

	    const int yes=1;

	    if ((rv = getaddrinfo(address.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
	        cerr << "getaddrinfo: " << gai_strerror(rv) << endl;
	        return false;
	    }

		// loop through all the results and bind to the first we can
	    for(p = servinfo; p != NULL; p = p->ai_next){
	        if ((sockfd = socket(p->ai_family, p->ai_socktype,
	                p->ai_protocol)) == -1) {
	            perror("server: socket");
	            continue;
	        }

	        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
	            perror("setsockopt");
	            exit(1);
	        }

	        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	            close(sockfd);
	            perror("server: bind");
	            continue;
	        }
	        break;
	    }

	    if (p == NULL)  {
	    	freeaddrinfo(servinfo); // all done with this structure
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

	bool set_up_listening(){
		if (listen(sockfd, BACKLOG) == -1) {
	        perror("listen");
	        throw runtime_error("listen error"); 
	        return false;
	    }
	    return true;
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
	TCP_Server server1(port, address);
	server1.server_accept();
    return 0;
}