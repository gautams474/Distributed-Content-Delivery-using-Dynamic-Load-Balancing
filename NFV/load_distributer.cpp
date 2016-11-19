#include <math.h>
#include <cstdlib>
#include <vector>
#include <string>

#include "load_distributer.h"

using namespace std;

struct server_data{
	int cores;
	vector<double> net_stats;
	vector<double> cpu_loads;
	vector<double> tps_parts;
};


Server::Server(){
	Server::tps_load = Server::getMax(Server::tps_parts);
	Server::net_out = Server::net_stats[1];
	Server::getPidWeight();	
}

void getWeightedLoad(Server &s){
	
	if(s.cpu_load > MAX_CPU_LOAD){
		s.load_percentage = 0;
		return;
	}
	else{
		if(s.net_out > MAX_NET_LOAD){
			s.load_percentage = 0;
			return;
		}
		else{
			if(s.tps_load > MAX_TPS_LOAD){
				s.load_percentage = 0;
				return;
			}
			// normalize parameters
			s.normalize();
			// allocate load %
			s.load_percentage = 0.4*s.tps_load + 0.4*s.net_out + 0.2*s.cpu_load;
		}
	} 
				
}

void getFileRange(Server &s, int &filecounter){
	s.file_start_index = filecounter;
	s.file_end_index = s.file_start_index + s.load_percentage;
	filecounter = s.file_end_index + 1;
}


int main(int argc, char** argv){

	Server s[3];
	int sum = 0, i, sum2 = 0;
	int filecounter = 0;
	for(i = 0; i < 3; i++){
		getWeightedLoad(s[i]);
		sum += s[i].load_percentage;
	}

	for(i = 0; i < 3; i++){
		s[i].load_percentage = sum/s[i].load_percentage;
		sum2 += s[i].load_percentage;
	}

	for(i = 0; i < 3; i++){
		s[i].load_percentage /= sum2;
		s[i].load_percentage = round(s[i].file_size*s[i].load_percentage);
	}

	for(i = 0; i < 3; i++){
		getFileRange(s[i],filecounter);
	}

}

