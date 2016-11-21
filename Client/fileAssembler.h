
using namespace std;

class File_Assembler{
public:

	File_Assembler(string file_name);

	/* Make file at specified path_name */
	bool makeFiles(string path_name);

	/* Make file at base_file_path */
	bool makeFiles(){return makeChunks(base_file_path);};

private:
	ofstream op_file;
	string file_name;
	string file_path;
	static const string base_file_path;
	static const int fileChunkLen = 64; // in bytes

	string getRealpath(string fileName);
	string getFileName(string fileName);
};

const string File_manipulator::base_file_path = "/home/prameet/ACN-Project/Client/"; 