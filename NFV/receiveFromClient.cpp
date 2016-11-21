#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>

#include <Sockets/tcp_socket.h>
#include <Sockets/udp_socket.h>
#include <NFV/load_distributer.h>
#include <NFV/NFV.h>

using namespace std;


bool NFV::receiveFromClient(string &packet, int &bytes){
	
	TCP_Socket nfv_client_server(NFV::PortToClient,NFV::NFV_IP,1);
	bool status = true;
	//int bytes;
	
	TCP_Socket client;
	status = nfv_client_server.server_accept(client);
	if(status == false){
		cout<<"server accept failed"<<endl;
	}
	else{

		status = nfv_client_server.receiveData((char *)packet.c_str(), 150, bytes);
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

bool NFV::broadcastToServer(UDP_Socket &socket, int bytes, string packet){
	bool ret;
	for(int i = 0; i< 3; i++){
		ret = socket.send_to((char *)packet.c_str(), 150, bytes, NFV::dest_port, NFV::server_ip[i]);
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
			return status;
		}
		i++;
	}
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
	string url;

	NFV nfv;
	UDP_Socket nfv_server(nfv.PortToServer,nfv.NFV_IP,1);

	// continuous thread 1
	while(1){
		ret = nfv.receiveFromClient(url,bytes);
		if(ret){
			// convert url to ip address
			// send to server;
			ret = nfv.broadcastToServer(nfv_server,bytes,url);
			if(ret){
				nfv.receiveLoadFromServer(nfv_server);
			}
		}
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