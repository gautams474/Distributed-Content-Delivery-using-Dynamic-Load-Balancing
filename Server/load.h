#include <vector>

using namespace std;
struct loadPacket
{
	// replace vectors with arrays
	vector<double> net_stats;
	vector<double> cpu_loads;
	vector<double> tps_parts;
	int file_size;
};

class Load{
public:
	vector<double> net_stats;
	vector<double> cpu_loads;
	vector<double> tps_parts;

	void getData(loadPacket &lpack, int fileChunks);
	
private:
	string split(string &s, char delim, int col);
	vector<double> getMetricFromFile(const char * filename, int offset, int col, int lines);
};