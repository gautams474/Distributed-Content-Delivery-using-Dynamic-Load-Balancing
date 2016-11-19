#include <vector>

using namespace std;

struct Server{
	int cores;
	vector<double> net_stats;
	vector<double> cpu_loads;
	vector<double> tps_parts;
	int load_percentage;
	int file_start_index;
	int file_end_index;
};

int main(int argc, char** argv){

	// for(int i=1;i<l.cpu_loads.size();i++){
	// 	if(cpu_loads[i] > 0.85)
	// 		continue;
	// }

	return 0;
}