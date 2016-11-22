#include <iostream>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <Sockets/tcp_socket.h>
#include <Sockets/udp_socket.h>
#include <Server/load.h>
#include <NFV/NFV.h>
	
using namespace std;

inline bool file_exists (const string& name) {
	return true;
	// struct stat buffer;   
	// return (stat (name.c_str(), &buffer) == 0  && S_ISDIR(buffer.st_mode)); 
}


/* Acting as a server to NFV */
void Server_NFV(int serverNo){
	const string base_path = "/home/prameet/ACN-Project/Content/";
	string my_port = NFV::server_port;//"10000";
	string my_address = NFV::server_ip[serverNo];//"10.3.15.5";
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

		url[bytes-1] = '\0';
		string s_url = url;
		s_url = base_path + s_url.substr(s_url.find("GET\r\n") + strlen("GET\r\n"));

		cout << "Checking for " << s_url << endl;
		if(file_exists(s_url) == false){
			//SEND NACK
			cout << "File not found. Sending NACK." << endl;
			loadPacket lpack;
			lpack.file_size = 0;
			if(server.send_to((char*) &lpack, sizeof(lpack), numBytes, dest_port, dest_address) == false){
				cout << "Could not send NACK succesfully. Sent " << bytes << " bytes of data." << endl;
				continue;
			}
			continue;
		}

		/************ Get File Chunks ************/
		/* TO DO Remove comments */
		// FILE *fp;
		// string command = "/bin/ls " + s_url + " -l | wc -l";

		// fp = popen(command.c_str(), "r");
		// if (fp == NULL) {
		// 	cerr << "Failed to run command\n";
		// 	cout << "Failed to run command\n";
		// 	continue;
		// }
		
		// int fileChunks;
		// fscanf(fp, "%d", &fileChunks);
		// pclose(fp);
		/* TO DO Remove below line */
		int fileChunks = 10;
		/*****************************************/

		/************* Load Calulate *************/
		loadPacket lpack;
		Load l;
		l.getData(lpack, fileChunks);
		/*****************************************/

		/************* Send load to nfv *************/
		if(server.send_to((char*) &lpack, sizeof(lpack), numBytes, dest_port, dest_address) == false){
			cout << "Could not send ACK succesfully. Sent " << bytes << " bytes of data." << endl;
			continue;
		}
		cout << "Sent ACK for " << url << endl;
		/*****************************************/
	}
}

int main(int argc, char** argv){

	cout << "Server num ? " << endl;
	int serverNo;
	cin >> serverNo;

	Server_NFV(serverNo);
	return 0;
}