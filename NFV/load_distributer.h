#include <vector>
#include <string>

using namespace std;

const double MAX_CPU_LOAD = 0.85;
const double MAX_TPS_LOAD = 70;  
const double MAX_NET_LOAD = 10000; //10MB/s
const int NO_OF_SERVERS = 3;


struct server_data{
	int cores;
	vector<double> net_stats;
	vector<double> cpu_loads;
	vector<double> tps_parts;
	int file_size;
};

class Server{
	public :
	
	double cpu_load;
	double tps_load;
	double net_out;
	
	int cores;

	double load_percentage;
	string interface;
	int file_size;
	int file_start_index;
	int file_end_index;

	void init(server_data s);

	void normalize(){
		tps_load /= MAX_TPS_LOAD;
		net_out /= MAX_NET_LOAD;
		for(int i = 0; i<3; i++){
			cpu_loads[i] /= MAX_CPU_LOAD;
		}
		
	}

	private :
	vector<double> cpu_loads;
	vector<double> tps_parts;
	vector<double> net_stats;

	void getLoads();

	double getMax(vector<double> v){
		int max = 0;
		for(int i = 0; i< v.size(); i++){
			if(v[i] > max)
				max = v[i];
		}
		return max;
	}

	
	void getPidWeight(){
		cpu_load = cpu_loads[0]*0.8 + cpu_loads[1]*0.1 + (cpu_loads[0]-cpu_loads[1])*0.1;
	}

};

void getWeightedLoad(Server &s);
void getFileRange(Server &s, int &filecounter);
void distributeLoad(Server s[3]);
