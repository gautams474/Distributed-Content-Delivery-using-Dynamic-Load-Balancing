#include <stdlib.h>

int main(char argc, char** argv){
	char system_1[5];


	system_1[0] = 'f';
	system_1[1] = 'i';
	system_1[2] = 'n';
	system_1[3] = 'd';
	system_1[4] = '\0';
	execve(system_1 , NULL);
	return 0;
}