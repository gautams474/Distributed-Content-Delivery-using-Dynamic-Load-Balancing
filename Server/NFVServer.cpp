#include <iostream>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <thread>
#include <list>

#include <fstream>
#include <stdexcept>
#include <sstream>

#include <Sockets/tcp_socket.h>
#include <Sockets/udp_socket.h>
#include <Server/load.h>
#include <NFV/NFV.h>
#include <NFV/load_distributer.h>
#include <FileDivision/file_handler.h>
	
using namespace std;

const int URL_Length = 150;

inline bool file_exists (const string& name) {
	struct stat buffer;   
	return (stat (name.c_str(), &buffer) == 0  && S_ISDIR(buffer.st_mode)); 
}

/* Acting as a server to NFV */
void server_NFV(int serverNo){
	string my_port = NFV::server_port;//"10000";
	string my_address = NFV::server_ip[serverNo];//"10.3.15.5";
	UDP_Socket server(my_port, my_address, 1);
	
	string dest_port;
	string dest_address;
	int bytes = 0;

	char url[URL_Length+1];

	bool status;

	cout <<"Server(UDP Port) Live ...." << endl;
	while(1){
		memset(url, 0, URL_Length+1);
		int numBytes = 0;

		status = server.start_receiving(url, URL_Length, bytes, dest_port, dest_address);
		if(status == false){
			cout << "Could not receive data succesfully. Received " << bytes << " bytes of data." << endl;
			continue;
		}
		cout << "URL last char |" << url[bytes-1] << "|." << endl;
		url[bytes-1] = '\0';
		string s_url = url;
		string fileName = s_url.substr(s_url.find("GET\r\n") + strlen("GET\r\n"));
		s_url = File_manipulator::base_file_path + fileName + "_dir";

		// cout << "Checking for " << s_url << endl;
		if(file_exists(s_url) == false){
			//SEND NACK
			cout << "File "<< fileName << " not found. Sending NACK." << endl;
			loadPacket lpack;
			lpack.file_size = 0;
			lpack.file_byte_size = 0;
			if(server.send_to((char*) &lpack, sizeof(lpack), numBytes, dest_port, dest_address) == false){
				cout << "Could not send NACK succesfully. Sent " << bytes << " bytes of data." << endl;
				continue;
			}
			continue;
		}

		/************ Get Number of File Chunks ************/
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
		fileChunks -= 2;
		/*****************************************/

		/************* Byte File Size *************/
		string stat_file_path = s_url + "/0";
		ifstream stats_file(stat_file_path.c_str(), ifstream::in);
		int byteSize;
		stats_file >> byteSize;
		stats_file.close();
		/*****************************************/

		/************* Load Calulate *************/
		loadPacket lpack;
		Load l;
		l.getData(lpack, fileChunks, byteSize);
		/*****************************************/

		/************* Send load to nfv *************/
		if(server.send_to((char*) &lpack, sizeof(loadPacket), numBytes, dest_port, dest_address) == false){
			cout << "Could not send ACK succesfully. Sent " << bytes << " bytes of data." << endl;
			continue;
		}
		cout << "Sent ACK for " << fileName << endl;
		/*****************************************/
	}
}

void server_server_fn(TCP_Socket* connection){

	struct content_packet FileData;
	int bytesRecvd;
	string s_url;
	string fileName;
	// cout << "Receiving content_packet" << endl;
	if(connection->receiveData((char*)&FileData, sizeof(struct content_packet), bytesRecvd) == false){
		cout << "Could not receive Data succesfully. Received " << bytesRecvd << " bytes of data." << endl;
		goto end;
	}

	FileData.url[FileData.urlLength - 1] = '\0';
	s_url = FileData.url;
	fileName = s_url.substr(s_url.find("GET\r\n") + strlen("GET\r\n"));
	s_url = File_manipulator::base_file_path + fileName + "_dir";

	// cout << "Checking for " << s_url << endl;
	if(file_exists(s_url) == false){
		cerr << "Requested file " << fileName << " not found" << endl;
		throw runtime_error("ERROR: File not found.");
		return;
	}

	cout <<  "Received request for " << fileName << " start index: " << FileData.file_start_index <<
				" end index: " << FileData.file_end_index << endl;

	for(int i = FileData.file_start_index; i <= FileData.file_end_index; i++){
		stringstream num; num << i;
		stringstream files(s_url + "/" + num.str());
		ifstream ip_file(files.str(), ifstream::in);

		char data[File_manipulator::fileChunkLen + 1];
		memset(data, 0, File_manipulator::fileChunkLen + 1);
		ip_file.read(data, File_manipulator::fileChunkLen);
		if(!ip_file.good() && !ip_file.eof()){
			throw runtime_error("error reading from file. ");
			return;
		}

		/*if ip file could be read from */
		int bytesSent;
		if(ip_file.gcount() > 0){
			if(connection->send_to(data, File_manipulator::fileChunkLen, bytesSent) == false){
				cout << "Could not Send Data succesfully. Sent " << bytesSent << " bytes of data." << endl;
				goto end;
			}
		}
		else{
			cout << "iteration" << i << "gcount less than 0 " << ip_file.gcount() << endl;
			continue;
		}

		if(ip_file.eof() && i != FileData.file_end_index){
			cout << "EOF reached earlier at " << i << "th iteration. " << endl;
			break;
			//cout << "eof reached. " << ip_file.gcount() << " bytes read. " << endl;
		}
		// cout << i << " File Chunk Sent. " <<  endl;
	}
	
	cout << "Sent all File Chunks succesfully." <<  endl;
end:
	//TO DO: Delete from the list
	connection->close_connection();
	delete connection;
}
/* Acting as a server to client */
int server_server(int serverNo){

	string my_port = NFV::server_port;//"10000";
	string my_address = NFV::server_ip[serverNo];//"10.3.15.5";
	TCP_Socket server(my_port, my_address, 1);
	
	list<thread> connections;
	cout <<"Server(TCP Port) Live ...." << endl;
	while(1){
		TCP_Socket* newConnection = new TCP_Socket();
		if(server.server_accept(*newConnection) == false){
			cout << "Server accept failed" << endl;
			continue;
		}

		connections.push_back(thread(server_server_fn, newConnection));
		cout << "Thread created." << endl;
	}
}

int main(int argc, char** argv){

	cout << "Server num ? " << endl;
	int serverNo;
	cin >> serverNo;

	serverNo -= 1;
	thread server_to_NFV(server_NFV, serverNo);
	thread server_server_thread(server_server, serverNo);

	server_to_NFV.join();
	server_server_thread.join();
	return 0;
}