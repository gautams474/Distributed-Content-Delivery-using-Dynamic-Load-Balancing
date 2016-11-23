#include <cstdlib>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <thread>

#include <Sockets/tcp_socket.h>
#include <Sockets/udp_socket.h>
#include <NFV/load_distributer.h>
#include <NFV/NFV.h>

using namespace std;

bool NFV::receiveFromClient(TCP_Socket &socket, TCP_Socket &client_socket, char *packet, int &bytes){
	bool status = true;
	//int bytes;
	
	status = socket.server_accept(client_socket);
	if(status == false){
		cout<<"server accept failed"<< endl;
	}
	else{
		cout<<"server socket accepted"<<endl;
		status = client_socket.receiveData(packet, 150, bytes);
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
	//Server servers[no_of_servers];
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
void NFV_Server_TCPSend(TCP_Socket *soc, content_packet *cpack){
	int bytes;
	bool status;
	char buf[64];
	cout << "sending cpack to servers." << endl;
	status = soc->send_to((char*)cpack, sizeof(content_packet), bytes);
	if(status == false){
		cout<<"send to server failed"<<endl;
		return;
	}
	while(1);
	status = soc->receiveData(buf, 64, bytes);
	if(status == false){
		cout<<"receive from server failed"<<endl;
		return;
	}
	cout << "Received " << bytes << " bytes of data." << endl;
	// delete [] cpack;
	// delete soc;
}

void NFV::getContentRequest(Server s[no_of_servers], char *url, int urlLength){

	content_packet *cpack = new content_packet[no_of_servers];
	//content_packet cpack[3];
	for(int i=0;i<no_of_servers;i++){
		if(s[i].load_percentage > 0){
			s[i].makeContentPacket(cpack[i], url, urlLength);
			TCP_Socket* server_soc = new TCP_Socket(s[i].port, s[i].ip_address, NFV::PortToServer, NFV::NFV_IP,0);
			thread th1(NFV_Server_TCPSend, server_soc, &cpack[i]);
			th1.join();
		}
	}
	//return true;
}

int main(){
	int bytes; // returned by receive from client, used by broadcast to server
	bool ret;
	char url[150];

	NFV nfv;
	
	TCP_Socket nfv_client_server(NFV::PortToClient,NFV::Client_NFV_IP,1);
	UDP_Socket nfv_server(nfv.PortToServer,nfv.NFV_IP,1);

	// continuous thread 1
	while(1){
		memset(url, 0, 150);
		TCP_Socket client;
		ret = nfv.receiveFromClient(nfv_client_server, client, url, bytes);
		if(ret){
			// convert url to ip address
			// send to server;
			cout<<"received " << bytes << " bytes from client"<<endl;
			int recvdbytes;
			ret = nfv.broadcastToServer(nfv_server, bytes, recvdbytes, url);
			if(ret){
				Server s_data[no_of_servers];
				cout<<"broadcast sent to all servers"<<endl;
				ret = nfv.receiveLoadFromServer(nfv_server,s_data);
				if(ret){
					int nbytes;
					int filesize = getMaxFileSize(s_data);
					ret = client.send_to((char*)&filesize,sizeof(int), nbytes);
					if(ret == false)
						cout<<"sending file size to client failed"<<endl;
					nfv.getContentRequest(s_data,url,bytes);
				}
			}
		}else{
			cout<<"receive failed"<<endl;
		}
		client.close_connection();
	}
	//testLoadDist();
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