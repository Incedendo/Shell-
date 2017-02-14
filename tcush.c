//*********************************************************
//
// Kiet Nguyen
// Operating Systems
// Project #1: Writing Your Own Shell: tcush
//
//*********************************************************


//*********************************************************
//
// Includes and Defines
//
//*********************************************************
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

#define STRMYQUIT "myquit"

#define MAX_CMD_LEN  128
#define HISTORY_COUNT 20


//*********************************************************
//
// Type Declarations
//
//*********************************************************
typedef enum{ false, true } bool;


//*********************************************************
//
// Extern Declarations
//
//*********************************************************
extern char **gettoks();


//*********************************************************
//
// Function Prototypes
//
//*********************************************************

//*********************************************************
//
// Function history: print maximum 20 strings of command
// previously entered by user
//
//*********************************************************
int history(char *hist[], int current)
{
        int i = current;
        int hist_num = 1;

        do {
                if (hist[i]) {
                        printf("%4d  %s\n", hist_num, hist[i]);
                        hist_num++;
                }

                i = (i + 1) % HISTORY_COUNT;

        } while (i != current);

        return 0;
}

//*********************************************************
//
// Function nls: list files and directories seperately
//
//*********************************************************
int ldir (void)
{
  DIR *dp;
  struct dirent *ep;     
  dp = opendir ("./");

  if (dp != NULL)
  {
    while ((ep = readdir (dp)) != NULL)
      puts (ep->d_name);

    (void) closedir (dp);
  }
  else
    perror ("Couldn't open the directory");

  return 0;
}

//*********************************************************
//
// Function ForWeb: change mode of subdir and files
//
//*********************************************************

//*********************************************************
//
// Main Function
//
//*********************************************************
int main( int argc, char *argv[] )
{
  	/* local variables */
  	int ii;
 	  char **toks;
  	int retval;

  	/* initialize local variables */
  	ii = 0;
  	toks = NULL;
  	retval = 0;

    char *hist[HISTORY_COUNT];
    int i, current = 0;
    char cmd[MAX_CMD_LEN];

    // Good programming habit to prevent bug
    // Set pointers to NULL right after declaration.
    for (i = 0; i < HISTORY_COUNT; i++)
              hist[i] = NULL;


  	/* put signal catching functions here */
  
  	/* main (infinite) loop */
  	while( true )
    {
      	/* get arguments */
      	toks = gettoks();

      	if( toks[0] != NULL )
    		{

          /* NULL to `free` is a NO-OP */
          free(hist[current]);
          
          strcpy(cmd,"");
          for(i=0;toks[i] != NULL; i++){            
            strcat(cmd, toks[i]);
            strcat(cmd," ");
          }

          hist[current] = strdup(cmd); //save the new command to history char*
          current = (current + 1) % HISTORY_COUNT; // inc counter

          int count = 0;
    		  /* simple loop to echo all arguments */
    		  for( ii=0; toks[ii] != NULL; ii++ )
    		    {
    		      printf( "Argument %d: %s\n", ii, toks[ii] );
              count = ii;
    		    }


    		  if( !strcmp( toks[0], STRMYQUIT ))
    		    break;

          if( !strcmp( toks[0], "history" ))
            {
                history(hist, current);
            }
          if( !strcmp( toks[0], "nls" ))
            {
                ldir();
            }
          if( count == 1){
            printf("Forweb func\n");
            if(!strcmp(toks[0],"forweb")){
              
            }
          }
    		}


    }

  	/* return to calling environment */
  	return(retval);
}
