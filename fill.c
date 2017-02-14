#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> //needed for open
#include <sys/stat.h>  //needed for open
#include <fcntl.h>     //needed for open
#include <unistd.h>

int main (int argc, char **argv){
	int i, last_space, fdfrom, length, fdto;
	int nl_pos;
	int count = 0;
	int prev = 0;
	int next = 0;
	int occupied = 0;
	int already_incur = 0;
	int n_char = 0 ;
	char buffer[132];
	int newLineFlag = 0;
	int spaceFlag = 0;
	int Spec_already_incur = 0;

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
	
	// tab = 3 spaces
	int num_read = 133;
	strcpy(buffer, "");
	printf("Read from %d then write to %d\n", fdfrom, fdto);
	while( (n_char = read(fdfrom, buffer, num_read)) > 0  )
        {	
        	already_incur += occupied;
        	printf("\nREAD: %d from target of %d, occupied value from last loop = %d, already_incur = %d\n",
        	 n_char, num_read, occupied, already_incur);
        	
        	newLineFlag = 0;
        	spaceFlag = 0;
        	nl_pos = 0;
        	//last_space = 0;
    		//traverse the buffer array and analyze the content:
			for(int i = 0; i < n_char; i++){

				if(count == 66){
					buffer[i] ='\f';
				}
				
				//2) Count the number of lines
				//   A form fed is added for every 66 lines from the previous form feed.
				if(buffer[i] == '\n'){
					//printf("new line read!!!\n");
					nl_pos = i;
					//Spec_already_incuralready_incur = num_read - nl_pos -1;
					already_incur = 0;
					printf("BC OF NEW LINE, already inc = %d\n", already_incur);
					newLineFlag = 1;
					//printf("ERASE ALL DEBT!!!\n");
					last_space = 0;
					occupied = 0;
					count++;
					// insert formfeed if 10 lines has been read.
					if(count == 10){
						//buffer[i] = '\f';
						count = 0;
					}
				}
				/*
				Find the last space closest to the buffer[132]
				Replace ' ' with \n and \n with ' '
				*/
				//Find last pos of space read.
				if(buffer[i] == ' '){
					printf("space at %d\n", i);
					spaceFlag = 1;
					last_space = i; //what if this space passes the max length
				}

				//1) All trailing blanks at the end of each line are removed.
				if((buffer[i] == ' ') && (buffer[i+1] == '\n'))
				{
					buffer[i] = '\0';
				}

				/*increment the length of the string as long as 
				the new line char is not read*/
				if( (buffer[i] != '\n') ){
					already_incur++;
					//printf("%d) Just read: %c\n",already_incur,buffer[i]);
					/*if length > 132, replace the closest space on the left
					with a new line char and reset*/
					if(already_incur == 133){
						//buffer[0] = '[';
						//buffer[i] = ']';
						occupied = 0;
						printf("\n[-->Num just read = %d, current index: %d = char: %c, debt from previous = %d\n", n_char,i, buffer[i], occupied);

						//printf("already_incur = %d\n", already_incur);
						
						//printf("-------");
						//if 1 '\n' already exist, do not enter second line.
						if( (last_space >= nl_pos) && (newLineFlag == 0) && (spaceFlag == 1) ) {
							buffer[last_space] = '\n';
							printf("-->Enter new line at %d\n", last_space);
						}

						already_incur = 0;
						count++;
						if( newLineFlag == 1){
							//everthing after new line is parsed to the next line and length < 132
							occupied = n_char-nl_pos-1;
							printf("Case new line\n");
							//printf("occupied = %d!\n", occupied);
						}
						if( newLineFlag == 0 && spaceFlag == 1){
							occupied = n_char-1-last_space;
							printf("last space = %d!\n", last_space);
						}
						printf("-->occupied<-- = %d]\n",occupied);
						//next = num_read-1-last_space

						num_read = 133 - occupied;
						printf("-->next debt: %d, num will read = %d\n",occupied,num_read);
						
						
					}else if(already_incur < 132){
						//there are less char than what is specified.
						num_read = 133 - already_incur;
						//printf("read EOL\n");
					}

				}
				//printf("reach\n");
			}

            //Display the characters read
            n_char = write(1, buffer, n_char);
			n_char = write(fdto, buffer, n_char);                              		   
        }
    printf("---------------\n");
	return 0;
}

