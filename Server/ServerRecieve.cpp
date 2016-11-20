#include <thread>
#include <string>

	string receivePort = "10000";
	vector<string> serverAddress;
	thread t[i];
	string url;
	int bytes;
	string NFV_port, 
	string NFV_address;
	/*
	for(int i=0; i<serverAddress.size();i++){
		UDP_Socket nfv_server("10000",serverAddress[i],1);
		t[i] = thread(receiverFromNfv, nfv_server);

	}*/

	
	void receiverFromNfv(){
		UDP_Socket server("10000",serverAddress[i],1);
		bool status;
		while(1){

			status = server.start_receiving(url.c_str(), 150, bytes, string& dest_port, string& dest_address);
			if(status){
				// load calculate
				// send load to nfv
			}
		}
	}