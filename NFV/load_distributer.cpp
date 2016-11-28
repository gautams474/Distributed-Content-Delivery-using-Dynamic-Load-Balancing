#include <math.h>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <NFV/load_distributer.h>

using namespace std;

void Server::init(loadPacket s, const string &ip, const string &my_port){
	
	memcpy(cpu_loads,s.cpu_loads,sizeof(s.cpu_loads));
	memcpy(net_stats, s.net_stats, sizeof(s.net_stats));
	disk_stat = s.disk_stat;
	file_size = s.file_size;
	file_byte_size = s.file_byte_size;
	cout << "File size = " << file_size << " .File byte size " << file_byte_size << endl;
	if(file_size == 0){
		cout << "got nack" << endl;
		cpu_load = 0;
		disk_stat = 0;
		net_out = 0;
		file_start_index = -1;
		file_end_index = -1;
	}
	else{
		getLoads();
	}
	
	ip_address = ip;
	port = my_port;
}

void Server::getLoads(){
	//tps_load = getMax(disk_stat);
	net_out = net_stats[1];
	getPidWeight();	
}

void Server::makeContentPacket(content_packet *cpack, char *url, int length){
	//cpack.url = url;
	memcpy(cpack->url,url,150);
	cpack->urlLength = length;
	cpack->file_start_index = file_start_index;
	cpack->file_end_index = file_end_index;
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
			if(s.disk_stat > MAX_TPS_LOAD){
				s.load_percentage = 0;
				return;
			}
			// normalize parameters
			s.normalize();
			// allocate load %
			s.load_percentage = 0.4*s.disk_stat + 0.4*s.net_out + 0.2*s.cpu_load;
			
		}
	} 
				
}

void getFileRange(Server &s, int &filecounter){
	if(s.load_percentage == 0)
		return;
	s.file_start_index = filecounter;
	s.file_end_index = s.file_start_index + s.load_percentage -1;
	filecounter = s.file_end_index + 1;

}


void distributeLoad(Server *s){
	double sum = 0, sum2 = 0, roundsum = 0;
	int i;
	int filecounter = 1;
	for(i = 0; i < 3; i++){
		getWeightedLoad(s[i]);
		sum += s[i].load_percentage;
	}

	for(i = 0; i < 3; i++){
		if(s[i].load_percentage == 0)
			continue;
		s[i].load_percentage = sum/s[i].load_percentage;
		sum2 += s[i].load_percentage;
	}

	for(i = 0; i < 3; i++){
		if(s[i].load_percentage == 0)
			continue;
		s[i].load_percentage /= sum2;
		if(i != 2){
			s[i].load_percentage = round(s[i].file_size*s[i].load_percentage);
			roundsum += s[i].load_percentage;
		}
		else{
			s[i].load_percentage = s[i].file_size - roundsum;
		}
		// cout<<"inside load_percentage "<<s[i].load_percentage<<endl;
	}

	for(i = 0; i < 3; i++){
		getFileRange(s[i],filecounter);
		cout << "server " << i << " start index :" << s[i].file_start_index <<" end index : "<< s[i].file_end_index << endl;
	}

}

int getMaxFileSize(Server *s){
	int max =0;
	for(int i = 0; i < 3; i++){
		if(s[i].file_byte_size > max)
			max = s[i].file_byte_size;
	}
	return max;
}

