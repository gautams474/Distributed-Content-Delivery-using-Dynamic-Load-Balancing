#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <fstream>
using namespace std;

class File_manipulator{
public:

	File_manipulator(string file_name);

	/* Make file at specified path_name */
	bool makeChunks(string path_name);

	/* Make file at base_file_path */
	bool makeChunks(){return makeChunks(base_file_path);};

	static const string base_file_path;
	static const int fileChunkLen = 64; // in bytes

private:
	ifstream ip_file;
	ofstream op_file;
	string file_name;
	string file_path;

	string getRealpath(string fileName);
	string getFileName(string fileName);
};

const string File_manipulator::base_file_path = "/users/prameets/Content/";
// const string File_manipulator::base_file_path = "Content/"; 

#endif