#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int commandWordCount = 0;

void  parse(char *line, char **argv)
{
     while (*line != '\0') {       /* if not the end of line ....... */
          commandWordCount++;
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     /* replace white spaces with 0    */
          *argv++ = line;          /* save the argument position     */
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;             /* skip the argument until ...    */
               
     }
     *argv = '\0';                 /* mark the end of argument list  */
}

void  execute(char **argv)
{
     pid_t  pid;
     int    status;

     if ((pid = fork()) < 0) {     /* fork a child process           */
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {          /* for the child process:         */
          if (execvp(*argv, argv) < 0) {     /* execute the command  */
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
     }
     else {                                  /* for the parent:      */
          while (wait(&status) != pid)       /* wait for completion  */
               ;
     }
}

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

int  main(void)
{
     char  line[1024];             /* the input line                 */
     char  *argv[64];              /* the command line argument      */

     while (1) {                   /* repeat until done ....         */

          printf("Shell -> ");     /*   display a prompt             */
          gets(line);              /*   read in the command line     */
          
          printf("\n");
          commandWordCount = 0;
          parse(line, argv);       /*   parse the line               */
          //printf("commandWordCount = %d\n",commandWordCount);
          if(commandWordCount > 0){
               if (strcmp(argv[0], "exit") == 0)  /* is it an "exit"?     */
               exit(0);            /*   exit if it is                */
          }
          execute(argv);           /* otherwise, execute the command */
          //initProcess(argv);
     }

     return 0;
}

/*if bg proc tries to read keyboard -> pause*/


