#include <vector>

using namespace std;
struct loadPacket
{
	// replace vectors with arrays
	double net_stats[2];
	double cpu_loads[3];
	double disk_stat;
	int file_size;
};

class Load{
public:
	vector<double> net_stats;
	vector<double> cpu_loads;
	vector<double> tps_parts;
	double disk_stat;

	void getData(loadPacket &lpack, int fileChunks);
	
private:
	double getMaxFromVector(vector<double> v);
	string split(string &s, char delim, int col);
	vector<double> getMetricFromFile(const char * filename, int offset, int col, int lines);
};