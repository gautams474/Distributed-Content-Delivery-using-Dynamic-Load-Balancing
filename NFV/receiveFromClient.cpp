#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>

#include "tcp_socket.h"
#include "udp_socket.h"
// #include "load_distributer.h"

using namespace std;

const string NFV_IP = "192.168.1.7";

bool receiveFromClient(string &packet){
	string portToClient = "10000";
	TCP_Socket nfv_client_server(portToClient,NFV_IP,1);
	bool status = true;
	int bytes;
	
	TCP_Socket client;
	status = nfv_client_server.server_accept(client);
	if(status == false){
		cout<<"server accept failed"<<endl;
	}
	else{

		status = nfv_client_server.receiveData((char *)packet.c_str(), 150, bytes);
		if(status == false){
			cout<<"server receive data failed"<<endl;
		}
		/*else if(bytes != sizeof(struct server_data)){
			cout<<"whole packet not copied"<<endl;
			status = false;
		}*/
	}

	return status;
}

int main(){
	string url;
	bool ret;
	int bytes;
	string server_ip;
	string portToServer = "10000";
	string dest_port = "10000";

	// continuous thread 1
	while(1){
		ret = receiveFromClient(url);
		if(ret){
			// convert url to ip address
			// send to server;
			UDP_Socket nfv_server(portToServer,NFV_IP,0);
			ret = nfv_server.send_to((char *)url.c_str(), 150, bytes, dest_port, server_ip);
			if(ret){
				cout<<"send failed"<<endl;
			}
		}
	}
}