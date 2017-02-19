//*********************************************************
//
// Kiet Nguyen [Final]
// Operating Systems
// Project #1: Writing Your Own Shell: tcush
//
//*********************************************************


//*********************************************************
//
// Includes and Defines
//
//*********************************************************
#include <ftw.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>

#define STRMYQUIT "myquit"
#define MAX_CMD_LEN  128
#define HISTORY_COUNT 10

char *hist[HISTORY_COUNT];
int i, current, actual, arg_count,sig_count = 0;


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
char** str_split(char* a_str, const char a_delim);
int fil (int arg_count, char src[], char dest[]);
int history(char *hist[], int current, int actual);
int ldir (void);
int forweb(const char *name, const struct stat *status, int type);
bool isInternal(char** toks);
void doExternal(char **toks, int count);
int do_N(char **toks, char cm[], int N);
int do_recent(char **toks);
void redirect(char **toks);
void runpipe(int pfd[], char**cmd1, char** cmd2);

//*********************************************************
//
// Function str_split: Split string with delimiter
// return: char**
//
//*********************************************************

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;
    
    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    
    /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
    count++;
    
    result = malloc(sizeof(char*) * count);
    
    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);
        
        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    
    return result;
}

//*********************************************************
//
// Function fill:
//
//*********************************************************
int fil (int arg_count, char src[], char dest[]){
    int i, last_space, fdfrom, length, fdto;
    
    int count = 0;
    
    int n_char = 0 ;
    char *buffer;
    char line[133];
    char line2[133];
    
    //
    int new_total;
    int consec_space;
    
    
    // tab = 3 spaces
    int num_read = 1;
    //buffer = 0;
    
    
    int pos = 0;
    int new_pos = 0;
    buffer = (char*)malloc(sizeof(char));
    
    for(i = 0; i < 133; i++)
        line[i] = 0;
    //strcpy(line,"");
    count = 0;
    consec_space = 0;
    
    
    fdfrom = open(src, O_RDONLY, 0644);
    if(!strcmp(dest, "stdout"))
        fdto = 1;
    else
        fdto = open(dest, O_CREAT|O_WRONLY, 0644);
    //printf("Read from %d then write to %d\n", fdfrom, fdto);
    
    int debt = 0;
    while( (n_char = read(fdfrom, buffer, 1)) > 0  )
    {
        int a;
        if(debt > 0){
            pos = new_pos;
            for(a = 0; a < new_pos; a++){
                line[a]  = line2[a];
            }
            new_pos = 0;
            debt = 0;
        }
        
        if(pos == 131) {
            line[pos++] = *buffer;
            
            //pos = 132
            debt = pos - last_space - 1;
            if (debt < 0) debt = 0;
            //printf("Last space pos = %d, debt = %d\n",last_space, debt);
            //last char at index pos-1
            
            //(last space+1, last space+2, last space+3,...,pos -1)
            new_pos = 0;
            if(debt > 0){
                for (a = last_space + 1; a < pos; a++){
                    printf("[%c]\n", line[a]);
                    line2[new_pos] = line[a];
                    line[a] = 0;
                    new_pos++;
                }
            }
            pos = new_pos;
            line[last_space] = '\n';
            
            //works from here
            write(1, line, 133);
            write(fdto, line, 133);
            pos = 0;
            consec_space = 0;
        }
        else{
            //printf("pos = %d\n", pos);
            if( *buffer == ' '){
                //printf("[space] consec_space = %d, pos = %d\n", consec_space, pos);
                line[pos++] = ' ';
                if(consec_space == 0){
                    last_space = pos - 1;
                    consec_space = 1;
                }else{
                    consec_space++;
                }
            }
            else if( *buffer == '\t'){
                //printf("[tab] consec_space = %d, pos = %d\n", consec_space, pos);
                line[pos++] = ' ';
                line[pos++] = ' ';
                line[pos++] = ' ';
                if(consec_space == 0)
                    consec_space = 3;
                else
                    consec_space += 3;
            }
            else if( *buffer == '\n'){
                //printf("[new line] consec_space b4 = %d\n", consec_space );
                if(consec_space > 0){
                    //printf("Pos before '\n' = %d\n", pos);
                    int ii = pos - 1;
                    int r = 0;
                    
                    while(line[ii] == ' '){
                        line[ii] = 0;
                        ii--;
                    }
                    consec_space = 0;
                }
                line[pos++] = '\n'; 
                count++;
                //printf("newline: count = %d\n",count);
                
                if(count == 5){
                    count = 0;
                    line[pos++] = '\f';     
                }
                for(i = pos; i < 133; i++){
                    line[i] = 0;
                }
                write(1, line, 133);
                write(fdto, line, 133);
                pos = 0;
            }
            else if(*buffer != '\n'){
                line[pos++] = *buffer;
                consec_space = 0;
            }   
        } 
        
    }
    //added after submission
    free(buffer);
    return 0;
}
//*********************************************************
//
// Function history: print maximum 10 strings of command
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
    DIR *ds;
    struct dirent *ep;
    dp = opendir ("./");
    ds = opendir ("./");
    char path[100];

    if (dp)
    {
        printf("DIRECTORY:\n");
        while ((ep = readdir (dp)) != NULL){
            if (ep->d_type == DT_DIR)
            {
                strcpy(path,ep->d_name);
                printf("Dir: %s\n", path);
 
            }
        }
        closedir (dp);
    }
    else
        perror ("Couldn't open the directory");
    
    
    //Put some space between the Directory and Regular Files
    printf("\n--------------------------------------------\n\n");
    
    if (ds)
    {
        printf("FILE:\n");
        while ((ep = readdir (ds)) != NULL){
            if (ep->d_type == DT_REG)
            {
                printf("File: %s\n", ep->d_name);

            }
        }
        closedir (ds);
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
int forweb(const char *name, const struct stat *status, int type){
    char *file;
    char res[50];
    char * pch;
    
    if(type == FTW_F){
         if( chmod(name , S_IROTH ) == -1 ) {
             perror(name);
         }
    }
    
    if(type == FTW_D && (strcmp(".", name) != 0)  ){ //&& (strcmp("..", name) != 0)
        if( chmod(name , S_IRUSR | S_IWUSR | S_IXUSR | S_IROTH | S_IXOTH ) == -1 ) {
            perror(name);
        }
    }
    return 0;
}

//*********************************************************
//
// Function doForweb()
//
//*********************************************************
int doForweb(int arg_count, char **toks) {
    if(arg_count == 1)
        ftw(".", forweb, 1);
    else
        ftw(toks[1], forweb, 1);
    
    printf("[doForweb] Done\n");
    return 0;
}

//*******************************************************************************************
//
// Function isInternal(): check if a command is a built-in command
//
//*******************************************************************************************

bool isInternal(char** toks){
    bool retval;
    int ii, isDup, task, file_pos;
    char dup_cmd[1000];
    
    strcpy(dup_cmd, "");
    
    if(!strcmp(toks[0],"!") || !strcmp(toks[0],"history") || !strcmp(toks[0],"forweb") || !strcmp(toks[0],"nls") || !strcmp(toks[0],"fil")){
        retval = true;
    }
    else{
        retval = false;
    }

    
    return retval;
}

//*****************************************************************************************************************************
//
// Function doInternal(char **toks): carry out 1 of the 4 internal commands or one of the BANG options
//
//*****************************************************************************************************************************
int doInternal(char **toks){
    char dest[1000];
    char src[1000];
    char cm[1000];
    
    if(!strcmp(toks[0],"!") ){
        //case: !!
        if(!strcmp(toks[1], "!")){
            do_recent(toks);
        }
        //case: !N
        else{
            //get N from !N
            int x = atoi(toks[1]);
            //check if N is a valid index in the table
            if(x >= actual){
                printf("Command out of valid bound\n");
                return 0;
            }
            else{
                //printf("Re-execute Command number %d which is %s\n",x, hist[(x-1)%10]);
                strcpy(hist[current-1], hist[(x-1)%10]);
                strcpy(cm,hist[(x-1)%10]);
                do_N(toks, cm, x);
            }
        }
    }else if( !strcmp(toks[0],"history")){
        history(hist, current,actual);
    }else if( !strcmp(toks[0],"forweb") ){
        doForweb(arg_count,toks);
    }else if( !strcmp(toks[0],"nls") ){
        ldir();
    }else if( !strcmp(toks[0],"fil")){
        printf("argc = %d\n",arg_count);
        
        /*
         check valid arg count, only continue if condition is TRUE:
            - fil redirection 
            - fil a.txt b.txt
         */
        if(arg_count > 1 && arg_count < 4){
            //printf("Args count for fill: %d\n", arg_count);
            
            //toks[1] is always provided => name of FROM ( either "-" or file name )
            strcpy(src, toks[1]);
            
            //printf("src: %s\n", toks[1]);
            if(arg_count == 3){
                strcpy(dest, toks[2]);  //dest is either "stdout" or "name of file"
            }else{
                strcpy(dest, "stdout");
            }
            //printf("dest: %s\n", dest);
            fil(arg_count,src,dest);
            
        }else{ //error handling
            if(arg_count <= 1)
                printf("Enter 1 or 2 files [current arg count = 1]\n");
            else
                printf("(> 3 arguments) Enter 2 valids file for [fil]\n");
        }
    }
    
    return 0;
}
//*************************************************************************************
//
// Function do_Recent(): this command re-execute the most recently executed command
//
//*************************************************************************************
int do_recent(char **toks){
    int new_current;
    char rec_cm[100];
    char **tokens;
    
    //test if there is actual recently executed command, else dont do anything and print Warning to stdout.
    if(current == 1){
        
        printf("No commands in history!\n");
        
    }else{
        //Jump to do recent command:
        // Replace the "!!" with the one right before it.
        hist[current-1] = hist[current-2];
        new_current = current - 2;
        
        //pass the Command to be excuted into [rec_cm], split it into tokens and store in [tokens]
        tokens = NULL;
        strcpy(rec_cm,hist[new_current]);
        tokens = str_split(rec_cm,' ');
        toks = tokens;
        
        if(isInternal(toks)){
            
            doInternal(toks);
            
        }else{
            
            doExternal(toks, arg_count);
        }
    }

    return 0;
}

//*********************************************************
//
// Function do_N()
//
//*********************************************************
int do_N(char **toks, char cm[], int N){
    int new_current;
    char **tokens;
    int i;
    
    if(N > actual){
        printf("No such commands in history!\n");
    }else{
        //Jump to do recent:
        
        // Re-execute Command number N
        
        tokens = NULL;
        tokens = str_split(cm,' ');
        int ind;
        toks = tokens;
        
        for(i = 0; toks[i] != NULL; i++){
            arg_count = i;
        }
        arg_count++;
        
        
        if(isInternal(toks)){
            doInternal(toks);
        }else{
            //printf("[do_N] Argc for external [%d]\n", arg_count);
            doExternal(toks,arg_count);
        }
    }
    
    return 0;
}

//*********************************************************
//
// Function do_external()
//
//*********************************************************
void doExternal(char **toks, int arg_count)
{
    int status = 0;
    
    //printf("count = [%d], last token: [%s]\n", arg_count,toks[arg_count-1]);
    
    int pid = fork();
    int childpids[10];
    int nChildren = 0;
    //printf("PID: %i\n", pid);
    int background = 0;
    
    
    //if(commandWordCount > 1){
    if(strcmp(toks[arg_count-1], "&") == 0) {
        //printf("background!!\n");
        background = 1;
        toks[arg_count-1] = NULL;
    }
    
    
    
    //printf("background: %i\n", background);
    if (pid == 0) {
        // child process
        if(background) {
            //printf("Child background process...");
            
            fclose(stdin); // close child's stdin
            fopen("/dev/null", "r"); // open a new stdin that is always empty
            execvp(*toks,toks);
            
            
            // If an error occurs, print error and exit
            printf( "unknown command: %s\n", toks[0]);
            exit(1);
            
        } else {
            //printf("Not Background\n");
            execvp(*toks,toks);
            
            // If an error occurs, print error and exit
            printf ( "unknown command: %s\n", toks[0]);
            exit(1);
        }
        
    } else {
        // parent process, waiting on child process
        
        if (background) {
            
            printf("starting background job %d\n", pid);
            childpids[nChildren] = pid;
            nChildren++;
            
        } else {
            printf("Not Background\n");
            waitpid(pid, &status, 0);
        }
        
        if (status != 0)                     //process exits if error occurs
            printf  ("error: %s exited with status code %d\n", toks[0], status);
    }
}

//*********************************************************
//
// Function sighandler:
//
//*********************************************************
void sighandler(int signum)
{
    sig_count++;
    printf("Caught signal %d, count = %d\n", signum, sig_count);
    //exit(1);
}

//*********************************************************
//
// Function runpipe:
//
//*********************************************************
void runpipe(int pfd[], char**cmd1, char** cmd2)
{
    int pid;
    
    switch (pid = fork()) {
            
        case 0: /* child */
            dup2(pfd[0], 0);
            close(pfd[1]);  /* the child does not need this end of the pipe */
            execvp(cmd2[0], cmd2);
            perror(cmd2[0]);
            
        default: /* parent */
            dup2(pfd[1], 1);
            close(pfd[0]);  /* the parent does not need this end of the pipe */
            execvp(cmd1[0], cmd1);
            perror(cmd1[0]);
            
        case -1:
            perror("fork");
            exit(1);
    }
}

//*********************************************************
//
// Function isPipe:
//
//*********************************************************
int doPipe(char **toks, int pipePos){
    
    char cmd1[1000];
    char cmd2[1000];
    int pid, status;
    int fd[2];
    int ii;
    
    printf("isPipe == 1\n");
    
    for(ii = 0; strcmp(toks[ii], "|") != 0; ii++){
        strcat(cmd1, toks[ii]);
        strcat(cmd1, " ");
    }
    //printf("cmd1 = %s\n",cmd1);
    
    for(ii = pipePos + 1; toks[ii] != NULL; ii++){
        strcat(cmd2, toks[ii]);
        strcat(cmd2, " ");
    }
    //printf("cmd2 = %s\n",cmd2);
 
    
    pipe(fd);
    
    switch (pid = fork()) {
            
        case 0: /* child */
            runpipe(fd, str_split(cmd1, ' '), str_split(cmd2, ' '));
            
        default: /* parent */
            while ((pid = wait(&status)) != -1)
                printf("process %d exits with %d\n", pid, WEXITSTATUS(status) );
            break;
            
        case -1:
            perror("fork");
            //exit(1);
    }

    return 0;
}

//*********************************************************
//
// Function Redirection:
//
//*********************************************************
int do_redirect(char **toks)
{
    int newfd, ii, dup_pos, task, file_pos;
    char dest[100];
    char src[100];
    char dup_cmd[100];
    
    //printf("enter redirect\n");
    
    if(!strcmp(toks[0], ">") || !strcmp(toks[0], "<") ){
        printf("[do_redirect] Incorrect format\n");
        return;
    }
    
    for( ii=1; toks[ii] != NULL; ii++ )
    {
        arg_count = ii;
    }
    arg_count++;
    
    for( ii=1; toks[ii] != NULL; ii++ )
    {
        
        if(!strcmp( toks[ii], ">" ) || !strcmp( toks[ii], "<" ))
        {
            if(ii < arg_count-1){
                file_pos = ii+1; // what if: "ls >" argc = 2; ii = 1; file_pos = 2; toks[2] = seg fault
            }else{
                file_pos = 0;
            }
            printf("[do_redirect] expected file_pos position: %d\n",file_pos);
                
            
            if(!strcmp( toks[ii], ">" )){
                /*
                 eg: 
                 history > out.txt
                 ls -l > out.txt
                 */
                //printf("task 1\n");
                task = 1;
                arg_count = ii; //arg_count for toks before ">"
                //printf("[do_redirect] command count (minus the [<...] part) = %d\n",arg_count );
            }
            else if(!strcmp( toks[ii], "<" )){
                //printf("task 2\n");
                task = 2;
            }
        }
    }
    
    /* Redirection Task Case 1: ">" parse result to the specified output*/
    
    if(task == 1){
        //printf("[In redirect] task 1\n");
        ii = 0;
        strcpy(dup_cmd, "");
        while( strcmp(toks[ii],">") != 0){
            strcat(dup_cmd, toks[ii]);
            strcat(dup_cmd, " ");
            ii++;
        }
        
        //printf("[do_redirect] file_pos = %d\n",file_pos);
        if(file_pos > 1){
            strcpy(dest, toks[file_pos]);
            //printf("[do_redirect] dest = %s\n",dest);

        }
        else{
            printf("please enter destination for redirection\n");
            return;
        }
        
        printf("(OUT TO) %s\n",dest );
        
        if ((newfd = open(dest, O_CREAT|O_TRUNC|O_WRONLY, 0644)) < 0) {
            perror(dest); /* open failed */
        }
        
        /*ls -l > out.txt*/
        //remove the > out.txt from toks
        for(ii = file_pos - 1; toks[ii] != NULL; ii++){
            toks[ii] = '\0';
        }
        
        //test
//        for(ii = file_pos - 1; toks[ii] != NULL; ii++){
//            printf("After >, toks aka command is: %s\n", toks[ii]);
//        }
        
        
        dup2(newfd, 1);
        return newfd;
        
        //execute the command: int or ext?
        
    }
    
    //• Example: thompson> sort < in.txt
    else if(task == 2){
        printf("[In redirect] task 2\n");
        //strcpy(src, toks[file_pos]);
        
        printf("(IN FROM) %s\n", src);
        
//        if ((newfd = open(src, O_RDONLY, 0644)) < 0) {
//            perror(dest); /* open failed */
//            //exit(1);
//        }

    }
    

}




//*********************************************************
//
// Global Variables
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
    int count; //argc
    char **toks;
    
    char **tokens;
    int retval;
    int isDup;
    int isPipe, pipePos;
    int task;
    int file_pos;
    int fd;
    int saved_stdout;
    
    int new_current;
    time_t rawtime;
    struct tm * timeinfo;
    
    /* initialize local variables */
    count = 0;
    ii = 0;
    toks = NULL;
    retval = 0;
    
    char cmd[MAX_CMD_LEN];
    
    
    // Good programming habit to prevent bug
    // Set pointers to NULL right after declaration.
    for (i = 0; i < HISTORY_COUNT; i++)
        hist[i] = NULL;
    
    /* put signal catching functions here */
    signal(SIGTSTP, sighandler);
    signal(SIGQUIT, sighandler);
    signal(SIGINT, sighandler);
    
    /* main (infinite) loop */
    while( true )
    {
        time ( &rawtime );
        timeinfo = localtime ( &rawtime );
        
        /*Set the shell's default message*/
        printf( "(%02d:%02d:%02d) Kiet's Shell: ",timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec );
        
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
            //printf("ORIGINAL CMD = %s\n", cmd);
            
            //
            hist[current] = strdup(cmd); //save the new command to history char*
            current = (current + 1) % HISTORY_COUNT; // inc counter
            actual++;
            
                        /* simple loop to echo all arguments */
            for( ii=0; toks[ii] != NULL; ii++ )
            {
                //printf("Argument %d: %s\n", ii, toks[ii] );
                arg_count = ii;
            
                //check if redirection is required.
                if(!strcmp( toks[ii], ">" ) || !strcmp( toks[ii], "<" )){
                    isDup = 1;
                    //printf("Redirect detected.\n");
                }
                
                if(!strcmp( toks[ii], "|" )){
                    isPipe = 1;
                    pipePos = ii;
                    //printf("Pipe detected at index [%d]!\n", pipePos);
                }
                
            }
            arg_count++;
            
            if( !strcmp( toks[0], STRMYQUIT )){
                break;
            }

            if(isDup == 1){
                
                /* Save current stdout for use later */
                saved_stdout = dup(1);
 
                fd = do_redirect(toks);
                //printf("redirect to [%d] done\n", fd);
            }
            
            if(isPipe != 1){
                if(isInternal(toks) ){ // && isPipe != 1
                    doInternal(toks);
                }else{
                    //printf("[main] do external for [%s], argc: [%d]\n",toks[0],arg_count);
                    doExternal(toks,arg_count);
                }
            }else{
                doPipe(toks, pipePos);
            }
            
            if(isDup == 1){
                /* Restore stdout */
                dup2(saved_stdout, 1);
                close(saved_stdout);

                isDup = 0;
                //printf("back to normal\n");
            }
            
            if(isPipe == 1){
                isPipe = 0;
            }
        }
        
        
    }
    
    /* return to calling environment */
    return(retval);
}
