/*
	output redirection with dup2()
	Super-simple example

	Paul Krzyzanowski
*/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char **argv)
{
	char cmd[1000];
	char dest[1000];
	char src[1000];
	char *inp[argc];
	int pid, status;
	int newfd;	/* new file descriptor */
	int task;

	// if (argc != 3) {
	// 	fprintf(stderr, "usage: %s output_file\n", argv[0]);
	// 	exit(1);
	// }
	int i;
	int pos = 0;
	printf("argc =%d\n", argc);
	for (i = 1; i < argc; i++){
		if( strcmp(argv[i],"out") == 0){
			task = 1;
			pos = i + 1;
			//printf("pos = %d\n", pos);
			break;
		}else if(strcmp(argv[i],"in") == 0){
			task = 2;
			pos = i + 1;
			//printf("pos = %d\n", pos);
			break;
		}else{
			strcat(cmd, argv[i]);
			strcat(cmd, " ");
		}
	}

	//pass in the file name to the dest string
	
	printf("cmd to redirect: %s\n",cmd);
	printf("destination file: %s\n",dest);

	if(task == 1){
		printf("(OUT)\n" );

		strcpy(dest, argv[pos]);

		if ((newfd = open(dest, O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
			perror(dest);	/* open failed */
			exit(1);
		}
		
		//fflush(stdout);

		/* this new file will become the standard output */
		/* standard output is file descriptor 1, so we use dup2 to */
		/* to copy the new file descriptor onto file descriptor 1 */
		/* dup2 will close the current standard output */

		dup2(newfd, 1); 
		
		//printf("\n\n\n\n\ntest\n");
		system(cmd);
	}

	//• Example: thompson> sort < in.txt
	if(task == 2){

		printf("(IN)\n");

		strcpy(src, argv[pos]);

		if ((newfd = open(src, O_RDONLY, 0644)) < 0) {
			perror(dest);	/* open failed */
			exit(1);
		}

	}else{
		system(cmd);
	}
	
	exit(0);
}

