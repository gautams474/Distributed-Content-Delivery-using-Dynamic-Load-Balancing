#include <iostream>
#include <string>
#include "./../Sockets/tcp_socket.h"

int main(int argc, char** argv){

	string my_ip = "10.0.3.15";
	string my_port = "11000";
	string dest_ip = "10.0.3.15";
	string dest_port = "10003";
	TCP_Socket client(dest_port, dest_ip, my_port, my_ip, false);

	string request = "GET\r\nmy_file.txt\r\n^";
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

	client.close_connection();
	// client.receiveData();
	return 0;
}