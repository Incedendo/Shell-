/*
 * C Program to List Files in Directory
 */
#include <dirent.h>
#include <stdio.h>
 
int main(void)
{
    DIR *d; //DIR is a directory stream => This is a pointer to a dir stream
    struct dirent *dir;
    d = opendir("."); //return pointer to a directory stream
    if (d)
    {
    /*The readdir() function returns a pointer to a structure representing 
    the directory entry at the current position in the directory stream 
    specified by the argument dirp, and positions the directory stream 
    at the next entry. It returns a null pointer upon reaching the end 
    of the directory stream. The structure dirent defined by the 
    <dirent.h> header describes a directory entry. */
        while ((dir = readdir(d)) != NULL)
            //Upon successful completion, readdir() returns 
            //a pointer to an object of type struct dirent
        {
            if (dir->d_type == DT_REG)
            {
             printf("%s\n", dir->d_name);
            }
        }
        closedir(d);
    }
    return(0);
}

//http://quiz.geeksforgeeks.org/c-program-list-files-sub-directories-directory/
