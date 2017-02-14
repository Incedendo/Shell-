/*
 * Change the permissions of a list of files
 * to by read/write by the owner only
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

int main( int argc, char **argv )
{
 
    char file[100] = "a";
    char *name = file;
 	if( chmod( file, S_IRGRP ) == -1 ) {
        perror(name);
        
    }

    return 0;
}