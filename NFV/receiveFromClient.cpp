#include <cstdlib>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <thread>
#include <list>
#include <algorithm>

#include <Sockets/tcp_socket.h>
#include <Sockets/udp_socket.h>
#include <NFV/load_distributer.h>
#include <NFV/NFV.h>

using namespace std;

bool NFV::receiveFromClient(TCP_Socket *client_socket, char *packet, int &bytes){
	bool status = true;
	//int bytes;

	cout<<"server socket accepted"<<endl;
	status = client_socket->receiveData(packet, 150, bytes);
	if(status == false){
		cout<<"server receive data failed"<<endl;
	}
	/*else if(bytes != sizeof(struct server_data)){
		cout<<"whole packet not copied"<<endl;
		status = false;
	}*/

	return status;
}

bool NFV::broadcastToServer(UDP_Socket &socket, int packet_len, int& bytes, char *packet){
	bool ret;
	for(int i = 0; i< no_of_servers; i++){
		ret = socket.send_to(packet, packet_len, bytes, NFV::server_port, NFV::server_ip[i]);
		if(ret == false){
			cout<<"send to server failed"<<endl;
			return ret;
		}
	}
	return ret;
}


bool NFV::receiveLoadFromServer(UDP_Socket &udp_socket, Server servers[no_of_servers]){
	int  i = 0;
	bool status;
	loadPacket s[no_of_servers];
	int bytes;
	string dest_port_recv[no_of_servers];
	string server_ip_recv[no_of_servers];
	while( i < no_of_servers){
		status = udp_socket.start_receiving((char *)&s[i], sizeof(loadPacket), bytes, dest_port_recv[i], server_ip_recv[i]);
		if(status == false){
			cout<<"NFV receive from Server failed"<<endl;
			return status;
		}
		cout<<"NFV received from server"<<endl;
		i++;
	}
	
	for(i =0 ; i<no_of_servers ; i++){
		servers[i].init(s[i],server_ip_recv[i],dest_port_recv[i]);
	}
	distributeLoad(servers);
	
	return true;
}

// thread function
void NFV_Server_TCPSend(TCP_Socket *server, content_packet *cpack, TCP_Socket* client, int filesize){
	int bytes = 0;
	cout << "sending cpack to servers." << endl;
	
	if(server->send_to((char*)cpack, sizeof(content_packet), bytes) == false){
		cout<<"send to server failed"<<endl;
		return;
	}
	const int fileChunkLen = 64;
	int bytes_remaining = filesize - ((cpack->file_start_index-1) * 64) - 
					max((filesize - ((cpack->file_end_index ) * 64)), 0);

	int numChunks = (filesize + fileChunkLen-1) / fileChunkLen;

	for(int i= cpack->file_start_index; i <= cpack->file_end_index; i++){
		char buf[fileChunkLen + 12];
		memset(buf, 0, fileChunkLen + 12);

		if(server->receiveData(buf, min(bytes_remaining, fileChunkLen) + 12, bytes) == false){
			cout << "Could not receive from server. Received " << bytes << " bytes of data." << endl;
			return;
		}
		cout << "Received " << bytes << " bytes of data from server. \t" << flush;

		if(client->send_to(buf, bytes, bytes) == false){
			cout << "Could not send to client. Sent " << bytes << " bytes of data." << endl;
			return;
		}
		cout << "Sent " << bytes << " bytes of data to client." << endl;
		bytes_remaining = bytes_remaining - fileChunkLen;
	}

	cout << "Sent all data bytes of data." << endl;
	delete cpack;
	delete server;
	//To Do: erase list
}

void NFV::getContentRequest(Server s[no_of_servers], char *url, int urlLength, list<thread> &threadList, TCP_Socket* client, int filesize){

	for(int i=0;i<no_of_servers;i++){
		if(s[i].load_percentage > 0){
			content_packet* cpack = new content_packet;
			s[i].makeContentPacket(cpack, url, urlLength);
			TCP_Socket* server_soc = new TCP_Socket(s[i].port, s[i].ip_address, NFV::PortToServer, NFV::NFV_IP,0);
			threadList.push_back(thread(NFV_Server_TCPSend, server_soc, cpack, client, filesize));
		}
	}
}

void do_join(thread& t){t.join();}

void manageServers(TCP_Socket* client){

	NFV nfv;
	UDP_Socket nfv_server(NFV::PortToServer, NFV::NFV_IP, 1);
	bool ret;
	int bytes = 0; // returned by receive from client, used by broadcast to server
	char url[150];
	memset(url, 0, 150);

	list<thread> tList;
	ret = nfv.receiveFromClient(client, url, bytes);
	if(ret){
		// To Do: convert url to ip address
		// send to server;
		cout<<"received " << bytes << " bytes from client"<<endl;
		int recvdbytes;
		ret = nfv.broadcastToServer(nfv_server, bytes, recvdbytes, url);
		if(ret){
			Server s_data[no_of_servers];
			cout << "broadcast sent to all servers" << endl;
			ret = nfv.receiveLoadFromServer(nfv_server, s_data);
			if(ret){
				int nbytes;
				int filesize = getMaxFileSize(s_data);
				ret = client->send_to((char*)&filesize,sizeof(int), nbytes);
				if(ret == false)
					cout<<"sending file size to client failed"<<endl;
				
				nfv.getContentRequest(s_data, url, bytes, tList, client, filesize);
			}
		}
	}else{
		cout<<"receive failed"<<endl;
	}

	cout << "Waiting for threads to join " << endl;
	for_each(tList.begin(), tList.end(), do_join);
	cout << "closing client connection client" << endl;
	client->close_connection();
	cout << "deleting client" << endl;
	delete client;

}

int main(){

	TCP_Socket nfv_client_server(NFV::PortToClient,NFV::Client_NFV_IP,1);
	bool status;
	// continuous thread 1
	// list<thread> tList;
	while(1){
		TCP_Socket* client = new TCP_Socket;
		status = nfv_client_server.server_accept(*client);
		if(status == false){
			cout<<"server accept failed"<< endl;
			delete client;
			continue;
		}
		thread t = thread(manageServers, client);
		t.detach();
	}
}
/*
void testLoadDist(){
	server_data s[3];
	Server servers[3];
	Load l;
	loadPacket lpack;
	for(int i=0;i<3;i++){
		l.getData(lpack);
		s[i].cpu_loads = lpack.cpu_loads;
		s[i].net_stats = lpack.net_stats;
		s[i].tps_parts = lpack.tps_parts;
		s[i].file_size = lpack.file_size;
		servers[i].init(s[i]);
	}
	distributeLoad(servers);
	for(int i=0;i<3;i++){
		cout<<"server "<<i<<endl;
		cout<<"cpu load "<<servers[i].cpu_load<<endl;
		cout<<"disk load "<<servers[i].tps_load<<endl;
		cout<<"net load "<<servers[i].net_out<<endl;
		cout<<"load percenatge "<<servers[i].load_percentage<<endl;
		cout<<"file start index "<<servers[i].file_start_index<<endl;
		cout <<"file end index "<<servers[i].file_end_index<<endl;
	}
}
*/