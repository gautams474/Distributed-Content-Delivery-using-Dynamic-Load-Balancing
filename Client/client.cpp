#include <iostream>
#include <string>
#include <Sockets/tcp_socket.h>
#include <NFV/NFV.h>

int main(int argc, char** argv){

	string client_ip =	NFV::Client_IP;
	string client_port = NFV::ClientPort;
	string NFV_ip = NFV::Client_NFV_IP;
	string NFV_port = NFV::PortToClient;

	TCP_Socket client(NFV_port, NFV_ip, client_port, client_ip, false);

	string request = "GET\r\nmy_file.txt^";
	int bytesSent = 0;
	if(client.send_to(request.c_str(), request.length(), bytesSent) == false){
		cout << "Could not send data succesfully. Sent " << bytesSent << " bytes." <<  endl;
		client.close_connection();
	}
	else if (bytesSent != request.length()){
		cout << "Could not send all data. Sent " << bytesSent << " bytes." <<  endl;
		client.close_connection();
	}
	cout << "Data Sent Succesfully." << endl;

	// char buf[64];
	// int bytesRecvd;
	// if(client.receiveData(buf, 64, bytesRecvd) == false)
	// 	cout << "Could not receive all data. Received " << bytesRecvd << " bytes." <<  endl;
	while(1);
	client.close_connection();
	// client.receiveData();
	return 0;
}