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
#include <fcntl.h>

#define STRMYQUIT "myquit"

#define MAX_CMD_LEN  128
#define HISTORY_COUNT 10

int sig_count = 0;
int commandWordCount;


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
// Function fill:
//
//*********************************************************
int fil (int arg_count, char src[], char dest[]){
    int i, last_space, fdfrom, length, fdto;
    int nl_pos;
    int line_count = 0;
    int prev = 0;
    int next = 0;
    int occupied = 0;
    int already_incur = 0;
    int n_char = 0 ;
    char buffer[132];
    int newLineFlag = 0;
    int spaceFlag = 0;
    int Spec_already_incur = 0;
    
    if(arg_count < 2 || arg_count > 3){
        printf("Follow format: fill [from] [to]\n"); //either TWO or THREE args
        exit(0);
    }
    
    printf("Read from %s, print to %s\n", src, dest);
    
    //Open the file to be read from
    if ((fdfrom = open(src, O_RDONLY, 0644)) < 0) {
        // if file doesnt exist, check if the arg is "-"
        if(!strcmp(src, "-")){
            fdfrom = 0; //stdinput
        }
        //else display error that the user fails to enter the valid file
        else{
            printf("Invalid From input\n");
            exit(0);
        }
    }
    
    // Missing [TO]: (fill [from]) -> stdout
    if(arg_count == 2){
        fdto = 1; //stdout
        printf("from = %d, to = %d\n", fdfrom, fdto);
    }
    
    // fil from to
    // fill
    if(arg_count == 3){
        if ((fdto = open(dest, O_CREAT|O_WRONLY, 0644)) < 0) {
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
        //printf("\nREAD: %d from target of %d, occupied value from last loop = %d, already_incur = %d\n",n_char, num_read, occupied, already_incur);
        
        newLineFlag = 0;
        spaceFlag = 0;
        nl_pos = 0;
        //last_space = 0;
        //traverse the buffer array and analyze the content:
        for(i = 0; i < n_char; i++){
            
            if(line_count == 66){
                buffer[i] ='\f';
            }
            
            //2) Count the number of lines
            //   A form fed is added for every 66 lines from the previous form feed.
            if(buffer[i] == '\n'){
                //printf("new line read!!!\n");
                nl_pos = i;
                //Spec_already_incuralready_incur = num_read - nl_pos -1;
                already_incur = 0;
                //printf("BC OF NEW LINE, already inc = %d\n", already_incur);
                newLineFlag = 1;
                //printf("ERASE ALL DEBT!!!\n");
                last_space = 0;
                occupied = 0;
                line_count++;
                // insert formfeed if 10 lines has been read.
                if(line_count == 66){
                    buffer[i] = '\f';
                    line_count = 0;
                }
            }
            /*
             Find the last space closest to the buffer[132]
             Replace ' ' with \n and \n with ' '
             */
            //Find last pos of space read.
            if(buffer[i] == ' '){
                //printf("space at %d\n", i);
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
                    //printf("\n[-->Num just read = %d, current index: %d = char: %c, debt from previous = %d\n", n_char,i, buffer[i], occupied);
                    
                    //printf("already_incur = %d\n", already_incur);
                    
                    //printf("-------");
                    //if 1 '\n' already exist, do not enter second line.
                    if( (last_space >= nl_pos) && (newLineFlag == 0) && (spaceFlag == 1) ) {
                        buffer[last_space] = '\n';
                        //printf("-->Enter new line at %d\n", last_space);
                    }
                    
                    already_incur = 0;
                    line_count++;
                    if( newLineFlag == 1){
                        //everthing after new line is parsed to the next line and length < 132
                        occupied = n_char-nl_pos-1;
                        //printf("Case new line\n");
                        //printf("occupied = %d!\n", occupied);
                    }
                    if( newLineFlag == 0 && spaceFlag == 1){
                        occupied = n_char-1-last_space;
                        //printf("last space = %d!\n", last_space);
                    }
                    //printf("-->occupied<-- = %d]\n",occupied);
                    //next = num_read-1-last_space
                    
                    num_read = 133 - occupied;
                    //printf("-->next debt: %d, num will read = %d\n",occupied,num_read);
                    
                    
                }else if(already_incur < 132){
                    //there are less char than what is specified.
                    num_read = 133 - already_incur;
                    //printf("read EOL\n");
                }
                
            }
            //printf("reach\n");
        }
        
        //Display the characters read
        //n_char = write(1, buffer, n_char);
        n_char = write(fdto, buffer, n_char);                              		   
    }
    //printf("---------------\n");
    return 0;
}

//*********************************************************
//
// Function sighandler:
//
//*********************************************************
void sighandler(int signum)
{
    sig_count++;
    printf("Caught signal %d, count = %d\n", signum,sig_count);
    //exit(1);
}

//*********************************************************
//
// Function &: run a process in the back ground
//
//*********************************************************
void initProcess(char **argv)
{
    int status = 0;
    int pid = fork();
    int childpids[10];
    int nChildren = 0;
    //printf("PID: %i\n", pid);
    int background = 0;
    
    if(commandWordCount > 1){
        if(strcmp(argv[commandWordCount-1], "&") == 0) {
            background = 1;
            argv[commandWordCount-1] = NULL;
            commandWordCount -= 1;
        }
    }
    
    
    //printf("background: %i\n", background);
    if (pid == 0) {
        // child process
        if(background) {
            //printf("Child background process...");
            
            fclose(stdin); // close child's stdin
            fopen("/dev/null", "r"); // open a new stdin that is always empty
            execvp(*argv,argv);
            
            
            // If an error occurs, print error and exit
            fprintf (stderr, "unknown command: %s\n", argv[0]);
            exit(1);
            
        } else {
            execvp(*argv,argv);
            
            // If an error occurs, print error and exit
            fprintf (stderr, "unknown command: %s\n", argv[0]);
            exit(1);
        }
        
    } else {
        // parent process, waiting on child process
        
        if (background) {
            
            printf("starting background job %d\n", pid);
            childpids[nChildren] = pid;
            nChildren++;
            
        } else {
            waitpid(pid, &status, 0);
        }
        
        if (status != 0)                     //process exits if error occurs
            fprintf  (stderr, "error: %s exited with status code %d\n", argv[0], status);
    }
}


//*********************************************************
//
// Function history: print maximum 20 strings of command
// previously entered by user
//
//*********************************************************
int history(char *hist[], int current, int actual)
{
        int i = current;
        int hist_num = 1;
        if(actual > 10)
            hist_num = actual - 9;

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
// Function systemCall: execute the funcion in the char[] cm
//
//*********************************************************

int systemCall(char cm[]){
    if(!strcmp(cm, "nls"))
        ldir();
//    if(!strcmp(cm, "history"))
//        history();
    
    return 0;
}

//*********************************************************
//
// Main Function
//
//*********************************************************
int main( int argc, char *argv[] )
{
  	/* local variables */
  	int ii;
    int count = 0;
    int fil_count;
    char **toks;
  	int retval;
    int dup;
    int task;
    int file_pos;
    int newfd;
    char dest[1000];
    char src[1000];
    char dup_cmd[1000];

  	/* initialize local variables */
  	ii = 0;
  	toks = NULL;
  	retval = 0;

    char *hist[HISTORY_COUNT];
    int i, current, actual = 0;
    char cmd[MAX_CMD_LEN];
    
    signal(SIGTSTP, sighandler);
    signal(SIGQUIT, sighandler);
    signal(SIGINT, sighandler);
    

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

            strcat(dup_cmd, toks[i]);
            strcat(dup_cmd," ");
          
            if(!strcmp( toks[i], ">" ) || !strcmp( toks[i], "<" )){
              dup = 1;
              file_pos = i+1;
              if(!strcmp( toks[i], ">" )){
                  printf("task 1\n");
                  task = 1;
              }
              if(!strcmp( toks[i], "<" )){
                  printf("task 2\n");
                  task = 2;
              }
            }else{
                dup = 0;
                task = 0;
            }
          }

          hist[current] = strdup(cmd); //save the new command to history char*
          current = (current + 1) % HISTORY_COUNT; // inc counter
          actual = actual + 1;
            
            /*compute length of command to determine if last char is &*/
            commandWordCount = strlen(cmd) - 1;
            printf("Last char of cmd = %c\n",cmd[commandWordCount-1]);
            
          /* simple loop to echo all arguments */
          for( ii=0; toks[ii] != NULL; ii++ )
            {
                printf( "Argument %d: %s\n", ii, toks[ii] );
                count = ii;
            }
            
            // call redirection
            if(dup == 1){
                printf("cmd to redirect: %s\n",cmd);
                //printf("destination file: %s\n",dest);
                if(task == 1){
                    
                    strcpy(dest, toks[file_pos]);
                    printf("(OUT TO) %s\n",dest );
                    
                    if ((newfd = open(dest, O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
                        perror(dest);	/* open failed */
                        exit(1);
                    }
                    
                    dup2(newfd, 1);
                    
                    printf("dup command: %s\n", dup_cmd);
                    systemCall(dup_cmd);
                }
                //• Example: thompson> sort < in.txt
                if(task == 2){
                    
                    strcpy(src, toks[file_pos]);
                    
                    printf("(IN FROM) %s\n", src);
                    
                    if ((newfd = open(src, O_RDONLY, 0644)) < 0) {
                        perror(dest);	/* open failed */
                        exit(1);
                    }
                    
                }else{
                    systemCall(dup_cmd);
                }   
    
            }


          if( !strcmp( toks[0], STRMYQUIT ))
            break;

          if( !strcmp( toks[0], "history" ))
            {
                history(hist, current, actual);
            }
            
          if( !strcmp( toks[0], "nls" ))
            {
                ldir();
            }

          if(!strcmp(toks[0], "!!"))
            {
                if(current == 0){
                    printf("No commands in history!\n");
                }else{
                    hist[current-1] = hist[current-2];
                    int new_current = current - 2;
                    printf("Most recent cm to Re-execute: %s\n", hist[new_current]);
                }
                
            }
          if(!strcmp(toks[0],"!"))
            {
                char number = toks[1];
                int x = number - '0';
                if(x == 0) x = 10;
                strcpy(hist[current-1], hist[x-1]);
                printf("Re-execute Command number %d which is %s\n",x, hist[x-1]);
        
            }
            
            /*
            -fil_count doesnt change
            -src doesnt change
            -dest doesnt change
            */
          if( !strcmp( toks[0], "fil" ))
            {
                fil_count = count + 1;
                if(fil_count > 1 && fil_count < 4){
                    printf("Args count for fill: %d\n", fil_count);
                    //toks[1] is always provided => name of FROM ( either "-" or file name )
                    strcpy(src, toks[1]);

                    if(fil_count == 3){
                        strcpy(dest, toks[2]);  //dest is either "stdout" or "name of file"
                    }else{
                        strcpy(dest, "stdout");
                    }
                    
                    fil(fil_count,src,dest);
                    
                }else{
                        printf("incorrect format for fil\n");
                }
            }
        }

    }

  	/* return to calling environment */
  	return(retval);
}
