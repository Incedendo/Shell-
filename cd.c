#include <stdio.h>
#include <unistd.h>
 
int main(int argc,char **argv) 
{ 
	char path[1000];
	getwd(path);
	printf("pass = %s\n",path);
	if (argc < 2)
	{  
		printf("Usage: %s <pathname\n",argv[0]);
		exit(1);
	}
	if (chdir(argv[1]) != 0)
	{ 
		printf("Error in chdir\n");
		exit(1);
	}
	return 0;
}