//Modified from page 7 of Interprocess Communication in Unix by John
//Shapely Gray 
//Usage: a.out filename
//Displays the contents of filename

//This program reads and prints to the screen

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h> //needed for open
#include <sys/stat.h>  //needed for open
#include <fcntl.h>     //needed for open
#include <string.h>

//using namespace std;

int main (int argc, char *argv[])
{
        int inFile;
        int n_char=0;
	char buffer[10];
        //char *buffers;
        //buffers = &buffer[0];
        char * pch;
        
        //char *input[]=
        // buffers = "quit";
        // printf("Print buffers: %s\n", buffers);
        
        //open the file to write input from stdin
        inFile = open(argv[1],O_CREAT|O_WRONLY, 0644);
        if (inFile == -1)
        {
                exit(1);
        }
        
        //execute the third argument
        //system(argv[2]);
        //exit(0);


        //Use the read system call to obtain 10 characters from inFile
        while( (n_char = read(inFile, (void *)buffer, 10)) > 0)
        {
                //Display the characters read
                n_char = write(0, buffer, n_char);
                printf("debug: no of char read = %d\n",n_char);
                buffer[n_char-1]='\0';
                //buffer[n_char] = '\n';
                if (strcmp(buffer, "quit") == 0){
                        close(inFile);
                        printf("equal\n");
                        break;
                }
                
                
                //if input is quit, then quit program.
                
                
        }

        

        return 0;
}