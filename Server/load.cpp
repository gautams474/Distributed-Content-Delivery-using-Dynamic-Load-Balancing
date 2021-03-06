#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <string.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <sstream>
#include <fstream>

#include <Server/load.h>

using namespace std;

const int cores = 1;
string Load::split(string &s, char delim, int col){
	//std::vector<std::string> elems;
	int i = 0;
	stringstream ss;
    ss.str(s);
    string item;
    while (std::getline(ss, item, delim)) {
    	if(!item.empty()){
    		i++;
        	//elems.push_back(item);
    		if(i == col)
    			break;
    	}
    }
    return item;
}

vector<double> Load::getMetricFromFile(const char * filename, int offset, int col, int lines){
	int i = 1;
	string line;
	ifstream fileInput;
	string column;
	double value;
	vector<double> columns;
	string::size_type sz;
	fileInput.open(filename,std::ifstream::in);
	if(fileInput.is_open()) {
	    while(!fileInput.eof()) {
	    	getline(fileInput, line);
	    	if(i >= offset){
	    		if(lines == 0)
		        	break;
	    		if(!line.empty()){
		        	column = split(line,' ', col);
		        	value = stod(column,&sz);		
		        	columns.push_back(value);
		        	//break;
		        	
	        	}
	        	lines--;
	    	}
	    	i++;	        
	    }
	    fileInput.close();
	    //return columns;
	}
	else cout << "Unable to open file.";

	return columns;
}

double Load::getMaxFromVector(vector<double> v){
	double max = 0, i;
	for(i =0 ; i<v.size() ; i++){
		if(v[i] > max)
			max = v[i];
	}
	return max;
}

void Load::getData(loadPacket &lpack, int fileChunks, int file_byte_size){

	system("./script.sh");

	Load::cpu_loads = getMetricFromFile("iolog.txt",1,1,3);

	vector<double> io_in =  getMetricFromFile("iolog.txt",6,5,1);
	vector<double> io_out = getMetricFromFile("iolog.txt",6,6,1);


	Load::net_stats.reserve( io_in.size() + io_out.size() ); // preallocate memory
	Load::net_stats.insert( net_stats.end(), io_in.begin(), io_in.end() );
	Load::net_stats.insert( net_stats.end(), io_out.begin(), io_out.end() );

	Load::tps_parts = getMetricFromFile("iolog.txt",13,2,-1);
	disk_stat = getMaxFromVector(tps_parts);

	for(int i=0;i<Load::cpu_loads.size();i++){
		Load::cpu_loads[i] /= cores;
	}	
	copy(cpu_loads.begin(), cpu_loads.end(), lpack.cpu_loads);
	copy(net_stats.begin(), net_stats.end(), lpack.net_stats);
	/*
	lpack.net_stats = Load::net_stats;
	lpack.cpu_loads = Load::cpu_loads;*/
	lpack.disk_stat = Load::disk_stat;

	// get no. of chunks for that file
	lpack.file_size = fileChunks;
	lpack.file_byte_size = file_byte_size;
}
/*
// test main
int main(){
	int i =0;
	int N = 1; // no.of CPU cores
	//vector<string> loads = exec("uptime | grep -o '[0-9]\\+\\.[0-9]\\+*'");
	Load l;
	
	l.getData();
	// give me l.lpack
}

*/



/*
std::vector<string> exec(const char* cmd) {
    char buffer[128];
    vector<string> result;
    int i = 0;
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL){
            	size_t ln = strlen(buffer)-1;
				if (buffer[ln] == '\n')
				    buffer[ln] = '\0';
                result.push_back(buffer) ;
            }
        	i++;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;

    // used from http://stackoverflow.com/questions/478898/how-to-execute-a-command-and-get-output-of-command-within-c-using-posix
}*/

 /* old main

	system("./script.sh");
	vector<double> cpu_loads = getMetricFromFile("iolog.txt",1,1,3);
	for(i=0;i<cpu_loads.size();i++){
		cout<<"cpu_load :"<<cpu_loads[i]<<endl;
	}	

	vector<double> io_in =  getMetricFromFile("iolog.txt",6,5,1);
	vector<double> io_out = getMetricFromFile("iolog.txt",6,6,1);
	vector<double> net_stats;

	net_stats.reserve( io_in.size() + io_out.size() ); // preallocate memory
	net_stats.insert( net_stats.end(), io_in.begin(), io_in.end() );
	net_stats.insert( net_stats.end(), io_out.begin(), io_out.end() );

	// http://stackoverflow.com/questions/3177241/what-is-the-best-way-to-concatenate-two-vectors

	for(i=0;i<net_stats.size();i++){
		cout<<"iostat :"<<net_stats[i]<<endl;
	}

	vector<double> tps_parts = getMetricFromFile("iolog.txt",13,2,-1);
	for(i=0;i<tps_parts.size();i++){
		cout<<"tps parts "<<tps_parts[i]<<endl;
	}
	
	return 0;
	*/

	/* test main
		for(i=0;i<l.cpu_loads.size();i++){
		cout<<"cpu_load :"<<l.cpu_loads[i]<<endl;
	}	

	for(i=0;i<l.net_stats.size();i++){
		cout<<"net_stat :"<<l.net_stats[i]<<endl;
	}

	for(i=0;i<l.tps_parts.size();i++){
		cout<<"tps parts "<<l.tps_parts[i]<<endl;
	}

	*/