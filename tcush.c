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
    int nl_pos, tab_pos;
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
            return 0;
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
            return 0;
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
            
            if(buffer[i] == '\t'){
                int tab_pos = i;
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
                //printf("%s\n", ep->d_name);
                strcpy(path,ep->d_name);
                printf("Dir: %s\n", path);
                // directory: read + execute: 5
                //int res = chmod(path, S_IXOTH | S_IROTH);
//                if( (strcmp(path,"..") != 0 ) && (strcmp(path,".") != 0 )){
//                    if( chmod(path , S_IROTH | S_IXOTH ) == -1 ) {
//                        perror(path);
//                    }
//                }
            }
        }
        closedir (dp);
    }
    else
        perror ("Couldn't open the directory");
    
    
    //2nd round
    printf("\n--------------------------------------------\n\n");
    
    if (ds)
    {
        printf("FILE:\n");
        while ((ep = readdir (ds)) != NULL){
            if (ep->d_type == DT_REG)
            {
                printf("File: %s\n", ep->d_name);
//                if( (strcmp(path,"..") != 0 ) && (strcmp(path,".") != 0 )){
//                    if( chmod(ep->d_name , S_IROTH ) == -1 ) {
//                        perror(path);
//                    }
//                }
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

//*********************************************************
//
// Function isInternal()
//
//*********************************************************

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

//*********************************************************
//
// Function isInternal()
//
//*********************************************************
int doInternal(char **toks){
    char dest[1000];
    char src[1000];
    char cm[1000];
    
    if(!strcmp(toks[0],"!") ){
        if(!strcmp(toks[1], "!")){
            do_recent(toks);
        }else{
            int x = atoi(toks[1]);
            if(x >= actual){
                printf("Such command does not exist\n");
                return 0;
            }
            else{
                printf("Re-execute Command number %d which is %s\n",x, hist[(x-1)%10]);
                strcpy(hist[current-1], hist[(x-1)%10]);
                strcpy(cm,hist[(x-1)%10]);
                do_N(toks, cm, x);
            }
            
        }
        
    }else if( !strcmp(toks[0],"history")){
        //printf("do history\n");
        history(hist, current,actual);
    }else if( !strcmp(toks[0],"forweb") ){
        printf("do forweb\n");
        doForweb(arg_count,toks);
    }else if( !strcmp(toks[0],"nls") ){
        ldir();
    }else if( !strcmp(toks[0],"fil")){
        printf("[do fil] ");
        printf("argc = %d\n",arg_count);
        if(arg_count > 1 && arg_count < 4){
            printf("Args count for fill: %d\n", arg_count);
            //toks[1] is always provided => name of FROM ( either "-" or file name )
            strcpy(src, toks[1]);
            
            if(arg_count == 3){
                strcpy(dest, toks[2]);  //dest is either "stdout" or "name of file"
            }else{
                strcpy(dest, "stdout");
            }
            fil(arg_count,src,dest);
            
        }else{
            if(arg_count <= 1)
                printf("Enter 1 or 2 files [current arg count = 1]\n");
            else
                printf("(> 3 arguments) Enter 2 valids file for [fil]\n");
        }
    }
    
    return 0;
}
//*********************************************************
//
// Function do_Recent()
//
//*********************************************************
int do_recent(char **toks){
    int new_current;
    char rec_cm[100];
    char **tokens;

    
    if(current == 1){
        printf("No commands in history!\n");
        
    }else{
        printf("Jump to do recent\n");
        hist[current-1] = hist[current-2];
        new_current = current - 2;
        printf("Most recent cm to Re-execute: %s\n\n", hist[new_current]);
        tokens = NULL;
        strcpy(rec_cm,hist[new_current]);
        tokens = str_split(rec_cm,' ');
        int ind;
        
//        //print out all the tokens
//        printf("Contents of [tokens]: ");
//        for(ind = 0; tokens[ind] != NULL; ind++){
//            printf("%s, ",tokens[ind]);
//        }
//        printf("\n");
//        
//        //print out all the toks
//        printf("Contents of [toks]: ");
//        for(ind = 0; toks[ind] != NULL; ind++){
//            printf("%s, ",toks[ind]);
//        }
//        printf("\n");
        
        toks = tokens;
        
        if(isInternal(toks)){
            doInternal(toks);
            printf("pass\n");
        }else{
            printf("[do_recent]\n");
            doExternal(toks, arg_count);
        }
        
//        printf("NEW TOKS : ");
//        for(ind = 0; toks[ind] != NULL; ind++){
//            printf("%s, ", toks[ind]);
//        }
//        printf("\n");
        
        
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
        printf("Jump to do recent: ");
        
        printf("Re-execute Command number %d which is %s\n",N, cm);
        //strcpy(rec_cm,cm);
        
        
        tokens = NULL;
        tokens = str_split(cm,' ');
        int ind;
        
        //        //print out all the tokens
        //        printf("Contents of [tokens]: ");
        //        for(ind = 0; tokens[ind] != NULL; ind++){
        //            printf("%s, ",tokens[ind]);
        //        }
        //        printf("\n");
        //
        //        //print out all the toks
        //        printf("Contents of [toks]: ");
        //        for(ind = 0; toks[ind] != NULL; ind++){
        //            printf("%s, ",toks[ind]);
        //        }
        //        printf("\n");
        
        toks = tokens;
        
        for(i = 0; toks[i] != NULL; i++){
            arg_count = i;
        }
        arg_count++;
        
        
        if(isInternal(toks)){
            doInternal(toks);
            printf("pass\n");
        }else{
            printf("[do_N] Argc for external [%d]\n", arg_count);
            doExternal(toks,arg_count);
        }
        
        //        printf("NEW TOKS : ");
        //        for(ind = 0; toks[ind] != NULL; ind++){
        //            printf("%s, ", toks[ind]);
        //        }
        //        printf("\n");
        
        
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
    
    printf("count = [%d], last token: [%s]\n", arg_count,toks[arg_count-1]);
    
    int pid = fork();
    int childpids[10];
    int nChildren = 0;
    //printf("PID: %i\n", pid);
    int background = 0;
    
    
    //if(commandWordCount > 1){
    if(strcmp(toks[arg_count-1], "&") == 0) {
        printf("background!!\n");
        background = 1;
        toks[arg_count-1] = NULL;
    }
    
    
    
    //printf("background: %i\n", background);
    if (pid == 0) {
        // child process
        if(background) {
            printf("Child background process...");
            
            fclose(stdin); // close child's stdin
            fopen("/dev/null", "r"); // open a new stdin that is always empty
            execvp(*toks,toks);
            
            
            // If an error occurs, print error and exit
            fprintf (stderr, "unknown command: %s\n", toks[0]);
            exit(1);
            
        } else {
            printf("Not Background\n");
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
// Function Redirection:
//
//*********************************************************
int do_redirect(char **toks)
{
    int newfd, ii, dup_pos, task, file_pos;
    char dest[100];
    char src[100];
    char dup_cmd[100];
    
    printf("enter redirect\n");
    
    if(!strcmp(toks[0], ">") || !strcmp(toks[0], "<") ){
        printf("[do_redirect] incorrect format\n");
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
                printf("task 1\n");
                task = 1;
                arg_count = ii; //arg_count for toks before ">"
                printf("[do_redirect] command count (minus the [<...] part) = %d\n",arg_count );
            }
            else if(!strcmp( toks[ii], "<" )){
                printf("task 2\n");
                task = 2;
            }
        }
    }
    
    if(task == 1){
        printf("[In redirect] task 1\n");
        ii = 0;
        strcpy(dup_cmd, "");
        while( strcmp(toks[ii],">") != 0){
            strcat(dup_cmd, toks[ii]);
            strcat(dup_cmd, " ");
            ii++;
        }
        
        printf("[do_redirect] file_pos = %d\n",file_pos);
        if(file_pos > 1){
            strcpy(dest, toks[file_pos]);
            printf("[do_redirect] dest = %s\n",dest);

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
        for(ii = file_pos - 1; toks[ii] != NULL; ii++){
            printf("After >, toks aka command is: %s\n", toks[ii]);
        }
        
        
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
            actual++;
            
            /* simple loop to echo all arguments */
            for( ii=0; toks[ii] != NULL; ii++ )
            {
                //printf( "(%s) Argument %d: %s\n",asctime (timeinfo), ii, toks[ii] );
                printf( "Argument %d: %s\n", ii, toks[ii] );
                arg_count = ii;
            
                //check if redirection is required.
                if(!strcmp( toks[ii], ">" ) || !strcmp( toks[ii], "<" )){
                    isDup = 1;
                    printf("Redirect detected.\n");
                }
                
            }
            arg_count++;
            
            
            
            if( !strcmp( toks[0], STRMYQUIT )){
                break;
            }
            
            if(isDup == 1){
                
                /* Save current stdout for use later */
                saved_stdout = dup(1);
                
                printf("reach\n");
                fd = do_redirect(toks);
                printf("redirect to [%d] done\n", fd);
            }
            
            if(isInternal(toks)){
                doInternal(toks);
            }else{
                printf("[main] do external for [%s], argc: [%d]\n",toks[0],arg_count);
                doExternal(toks,arg_count);
            }
            
            if(isDup == 1){
                /* Restore stdout */
                dup2(saved_stdout, 1);
                close(saved_stdout);

                isDup = 0;
                printf("back to normal\n");
            }

        }
        
        
    }
    
    /* return to calling environment */
    return(retval);
}
