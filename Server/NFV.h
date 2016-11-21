
class NFV{

public: 
	const string NFV_IP = "192.168.1.7";
	const string PortToServer = "10000";
	const string PortToClient = "11000";
	const string server_ip[3] = {"","",""};
	const string dest_port="10000";

	/*NFV(string server_ip_array[3], string server_port){
		for(int i = 0; i < 3; i++){
			server_ip[i] = server_ip_array[i];
		}
		dest_port = server_port;
	}*/

	bool receiveFromClient(string &packet, int &bytes);

	bool broadcastToServer(UDP_Socket &socket, int bytes, string packet);

	bool receiveLoadFromServer(UDP_Socket &socket);

};

