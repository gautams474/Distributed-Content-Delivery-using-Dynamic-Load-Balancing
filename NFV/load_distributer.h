#ifndef LOAD_DISTRIBUTER_H
#define LOAD_DISTRIBUTER_H

#include <vector>
#include <string>

using namespace std;

const double MAX_CPU_LOAD = 0.85;
const double MAX_TPS_LOAD = 70;  
const double MAX_NET_LOAD = 10000; //10MB/s
const int NO_OF_SERVERS = 3;

struct server_data{
	double net_stats[2];
	double cpu_loads[3];
	double disk_stat;
	int file_size;
};

struct content_packet{
	char url[150];
	int urlLength;
	int file_start_index;
	int file_end_index;
};

class Server{
	public :
	double disk_stat;
	double cpu_load;
	double net_out;	

	double load_percentage;

	string ip_address;
	string port;
	
	int file_size;
	int file_start_index;
	int file_end_index;

	void init(server_data s,const string &ip, const string &my_port);
	void makeContentPacket(content_packet &cpack, char *url, int length);

	void normalize(){
		if(file_size == 0)
			return;
		disk_stat /= MAX_TPS_LOAD;
		net_out /= MAX_NET_LOAD;
		for(int i = 0; i < 3; i++){
			cpu_loads[i] /= MAX_CPU_LOAD;
		}	
	}

	private :
	double net_stats[2];
	double cpu_loads[3];
	

	void getLoads();
	/*
	double getMax(vector<double> v){
		int max = 0;
		for(int i = 0; i< v.size(); i++){
			if(v[i] > max)
				max = v[i];
		}
		return max;
	}
	*/
	
	void getPidWeight(){
		cpu_load = cpu_loads[0]*0.8 + cpu_loads[1]*0.1 + (cpu_loads[0]-cpu_loads[1])*0.1;
	}

};

void getWeightedLoad(Server &s);
void getFileRange(Server &s, int &filecounter);
void distributeLoad(Server s[3]);
int getMaxFileSize(Server *s);

#endif
