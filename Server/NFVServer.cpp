#include <iostream>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <Sockets/udp_socket.h>
#include <Server/load.h>
	
using namespace std;

inline bool file_exists (const string& name) {
	struct stat buffer;   
	return (stat (name.c_str(), &buffer) == 0  && S_ISDIR(buffer.st_mode)); 
}


/* Acting as a server to NFV */
void Server_NFV(){
	const string base_path = "/home/prameet/ACN-Project/Content/";
	string my_port = "10000";
	string my_address = "10.3.15.5";
	UDP_Socket server(my_port, my_address, 1);
	
	string dest_port;
	string dest_address;
	int bytes = 0;

	const int URL_Length = 150;
	char url[URL_Length];

	bool status;

	cout <<"Server Live ...." << endl;
	while(1){
		memset(url, 0, URL_Length+1);
		int numBytes = 0;

		status = server.start_receiving(url, URL_Length, bytes, dest_port, dest_address);
		if(status == false){
			cout << "Could not receive data succesfully. Received " << bytes << " bytes of data." << endl;
			continue;
		}

		url[bytes] = '\0';
		string s_url = url;
		s_url = base_path + s_url.substr(s_url.find("GET\r\n") + strlen("GET\r\n"));
		if(file_exists(s_url) == false){
			//SEND NACK
			loadPacket lpack;
			lpack.file_size = 0;
			if(server.send_to((char*) &lpack, sizeof(lpack), numBytes, dest_port, dest_address) == false){
				cout << "Could not send NACK succesfully. Sent " << bytes << " bytes of data." << endl;
				continue;
			}
		}

		/************ Get File Chunks ************/
		FILE *fp;
		string command = "/bin/ls " + s_url + " -l | wc -l";

		fp = popen(command.c_str(), "r");
		if (fp == NULL) {
			cerr << "Failed to run command\n";
			cout << "Failed to run command\n";
			continue;
		}
		
		int fileChunks;
		fscanf(fp, "%d", &fileChunks);
		pclose(fp);
		/*****************************************/

		/************* Load Calulate *************/
		loadPacket lpack;
		Load l;
		l.getData(lpack, fileChunks);
		/*****************************************/

		/************* Send load to nfv *************/
		if(server.send_to((char*) &lpack, sizeof(lpack), numBytes, dest_port, dest_address) == false){
			cout << "Could not send NACK succesfully. Sent " << bytes << " bytes of data." << endl;
			continue;
		}
		cout << "Sent ACK for " << url << endl;
		/*****************************************/
	}
}

int main(int argc, char** argv){

	Server_NFV();
	return 0;
}