
using namespace std;

class File_manipulator{
public:

	File_manipulator(string file_name);

	bool makeChunks(string path_name);
	bool makeChunks(){return makeChunks(file_path);}

private:
	ifstream ip_file;
	string file_name;
	string file_path;
	static const int fileChunkLen = 64; // in bytes

	string getRealpath(string fileName);
	string getFileName(string fileName);
};