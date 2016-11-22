#include <cstdlib>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>

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
	for(int i = 0; i< 3; i++){
		ret = socket.send_to(packet, packet_len, bytes, NFV::server_port, NFV::server_ip[i]);
		if(ret == false){
			cout<<"send to server failed"<<endl;
			return ret;
		}
	}
	return ret;
}


bool NFV::receiveLoadFromServer(UDP_Socket &socket){
	int no_of_servers = 3, i = 0;
	bool status;
	server_data s[3];
	Server servers[3];
	int bytes;
	string dest_port_recv[3];
	string server_ip_recv[3];
	while( i < no_of_servers){
		status = socket.start_receiving((char *)&s[i], sizeof(server_data), bytes, dest_port_recv[i], server_ip_recv[i]);
		if(status == false){
			cout<<"NFV receive from Server failed"<<endl;
			return status;
		}
		cout<<"NFV received from server"<<endl;
		i++;
	}
	//check for nack
	for(i =0 ; i<3 ; i++){
		servers[i].init(s[i],server_ip_recv[i],dest_port_recv[i]);
	}
	distributeLoad(servers);
	// send get Content request to servers
	return true;
}

//void testLoadDist();

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
				cout<<"broadcast sent to all servers"<<endl;
				nfv.receiveLoadFromServer(nfv_server);
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