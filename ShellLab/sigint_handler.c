#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

void sighandler(int);
int count = 0;

int main()
{
	//int count;
   //signal(SIGINT, sighandler);
   signal(SIGTSTP, sighandler);
   signal(SIGQUIT, sighandler);

   while(1) 
   {
      printf("Going to sleep for a second...\n");
      sleep(1); 
   }
   return(0);
}

void sighandler(int signum)
{
   count++;
   printf("Caught signal %d, count = %d\n", signum,count);
   //exit(1);
}