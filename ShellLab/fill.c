#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> //needed for open
#include <sys/stat.h>  //needed for open
#include <fcntl.h>     //needed for open
#include <unistd.h>

int main (int argc, char **argv){
	int i, fdfrom, fdto;
	int count = 0;
	int n_char=0;
	char buffer[132];
	char formfeed[2];
	//strcpy(formfeed, "--");

	if(argc < 2 || argc > 3){
		printf("Follow format: fill [from] [to]\n");
	}

	//Open the file to be read from
	if ((fdfrom = open(argv[1], O_RDONLY, 0644)) < 0) {
		// if file doesnt exist, check if the arg is "-"
		if(!strcmp(argv[1], "-")){
			fdfrom = 0; //stdinput
		}
		//else display error that the user fails to enter the valid file
		else{
			printf("Invalid From input\n");
			exit(0);
		}
	}

	// Missing to: (fill [from]) -> stdout
	if(argc == 2){
		fdto = 1; //stdout
		printf("from = %d, to = %d\n", fdfrom, fdto);
	}

	// fil from to
	// fill 
	if(argc == 3){
		if ((fdto = open(argv[2], O_CREAT|O_WRONLY, 0644)) < 0) {
			printf("Invalid To input: File does not exist.\n");
			exit(0);
		}
	}
	printf("reach\n");
	// tab = 3 spaces
	printf("Read from %d then write to %d\n", fdfrom, fdto);
	while( (n_char = read(fdfrom, buffer, 132)) > 0  )
        {
     
    		printf("Read from %d then write to %d\n", fdfrom, fdto);

    		//traverse the buffer array and analyze the content:
			for(int i = 0; i < n_char; i++){
				//1) if any space is followed by \n, replace ' ' with ''
				if((buffer[i] == ' ') && (buffer[i+1] == '\n'))
				{
					buffer[i] = '\0';
				}

				//2) count the number of lines
				if(buffer[i] == '\n'){
					count++;
					// insert formfeed if 10 lines has been read.
					if(count == 10){
						buffer[i] = '\f';
						count = 0;
					}
				}
			}

				

            //Display the characters read
            n_char = write(fdto, buffer, n_char);

            //write(fdto,space,1)
            // if(buffer[n_char-1] == ' '){
            // 	buffer[n_char-1] = 0;
            // } 
        	
        		   
        }
	return 0;
}