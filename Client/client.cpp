#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <Sockets/tcp_socket.h>
#include <FileDivision/file_handler.h>
#include <NFV/NFV.h>
#include <sys/stat.h>
#include <chrono>

int main(int argc, char** argv){

	const int FileChunkLen = 64;

	string client_ip =	NFV::Client_IP;
	string client_port = NFV::ClientPort;
	string NFV_ip = NFV::Client_NFV_IP;
	string NFV_port = NFV::PortToClient;

	TCP_Socket client(NFV_port, NFV_ip, client_port, client_ip, false);

	string delim = "^";
	string file = "my_file.txt";
	string request = "GET\r\n" + file + delim;
	// cout << "Request last character " << request.at(request.length() - 1) << endl;
	int bytesSent = 0;
	cout << chrono::high_resolution_clock::period::den << endl;
	auto start_time = chrono::high_resolution_clock::now();
	if(client.send_to(request.c_str(), request.length(), bytesSent) == false){
		cout << "Could not send data succesfully. Sent " << bytesSent << " bytes." <<  endl;
		client.close_connection();
	}
	else if (bytesSent != request.length()){
		cout << "Could not send all data. Sent " << bytesSent << " bytes." <<  endl;
		client.close_connection();
	}
	cout << "Request Sent Succesfully." << endl;

	int bytesRecvd = 0;
	int fileSize;
	if(client.receiveData((char*)&fileSize, sizeof(int), bytesRecvd) == false){
		cout << "Could not receive all data. Received " << bytesRecvd << " bytes." <<  endl;
		client.close_connection();
		return -1;
	}
	else{
		cout << "fileSize: " << fileSize << " bytes." << endl;
	}

	string path = "/users/prameets/Content/";
	string dir = "my_file.txt_dir";
	string chunk_path = path + dir;
	int ret = mkdir(chunk_path.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
	if(ret != 0){
		perror("Could not make directory: ");
		return false;
	}

	int numChunks = (fileSize + FileChunkLen-1) / FileChunkLen;
	int bytes_remaining = fileSize;
	int lastChunkSize = fileSize - ((numChunks-1)*FileChunkLen);
	for(int i=0; i< numChunks; i++){

		bytesRecvd = 0;
		char data[FileChunkLen];
		memset(data, 0, FileChunkLen);

		char s_fileNum[13];
		memset(s_fileNum, 0, 13);
		// cout << "Receiving 12 bytes " <<  flush;
		if(client.receiveData(s_fileNum, 12, bytesRecvd) == false){
			cout << "Could not receive all data. Received " << bytesRecvd << " bytes." <<  endl;
			client.close_connection();
			return -1;
		}

		// cout << "Converting fileNum to unsigned Long " << s_fileNum << flush;
		bytesRecvd = 0;
		unsigned long long int fileNum = 0;
		char* end;
		fileNum = strtoull (s_fileNum, &end, 10);
		// cout << "Converted to " << fileNum << " " << flush;

		// sscanf(s_fileNum, "%10llu", &fileNum);

		if(fileNum == numChunks){
			// cout << "last Chunk of size " << lastChunkSize << " " << flush;
			if(client.receiveData(data, lastChunkSize, bytesRecvd) == false){
				cout << "Could not receive all data. Received " << bytesRecvd << " bytes." <<  endl;
				client.close_connection();
				return -1;
			}
		}
		else{
			if(client.receiveData(data, min(FileChunkLen, bytes_remaining), bytesRecvd) == false){
				cout << "Could not receive all data. Received " << bytesRecvd << " bytes." <<  endl;
				client.close_connection();
				return -1;
			}
		}

		// cout << "Received Data." << flush;
		stringstream num; num << fileNum; //num << fileNum;
		stringstream files(chunk_path + "/" + num.str());

		ofstream op_file(files.str().c_str(), ios::binary | ios::out);
		op_file.write(data, bytesRecvd);
		op_file.close();
		
		bytes_remaining -= bytesRecvd; //FileChunkLen;
		// cout << "File written. " << bytes_remaining << " bytes remaining." << endl;
	}
	
	cout << "Received All Files" << endl;
	
	/*Assembling the file chunks*/
	string s_op_file = path + file;
	ofstream op_file(s_op_file.c_str(), ios::binary | ios::out);
	for(int i=0; i< numChunks; i++){
		stringstream num; num << i+1;
		stringstream files(chunk_path + "/" + num.str());
		ifstream ip_file_chunk(files.str().c_str(), ios::binary | ios::out);

		op_file << ip_file_chunk.rdbuf();
	}

	string remove = "rm -rf ";
	string command = remove + chunk_path.c_str();
	system(command.c_str());

	cout << "Assembled Files" << endl;
	auto end_time = chrono::high_resolution_clock::now();
	cout << chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count() << ":";
	// To Do: Merge Files
	client.close_connection();
	// client.receiveData();
	return 0;
}