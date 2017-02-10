#include <stdio.h>
#include <string.h>

    int main()
    {

       char command[10];
       scanf("%s",command);

       // command and "hello" can be less than, equal or greater than!
       // thus, strcmp return 3 possible values
       if (strcmp(command, "hello") == 0)
       {
          printf("\nThe user said hello!\n");
       }

       return 0;

    }