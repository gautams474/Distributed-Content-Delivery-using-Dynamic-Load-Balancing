#include <thread>
#include <string>
#include <cstring>
	

/* Acting a server to NFV */
void Server_NFV(){
	string my_port = "10000";
	string my_address = "10.3.15.5";
	UDP_Socket server(my_port, my_address, 1);
	
	string dest_port;
	string dest_address;
	int bytes;

	const int URL_Length = 150;
	char url[URL_Length];

	bool status;

	while(1){
		memset(url, 0, URL_Length+1);

		status = server.start_receiving(url, URL_Length, bytes, dest_port, dest_address);
		if(status == false){
			cout << "Could not receive data succesfully. Received " << bytes << " bytes of data." << endl;
			continue;
		}
		
		// load calculate


		// send load to nfv

	}
}