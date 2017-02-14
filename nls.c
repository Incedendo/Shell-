#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h> 

int main (void)
{
  DIR *dp;
  DIR *ds;
  struct dirent *ep;      
  dp = opendir ("./");  
  ds = opendir ("./"); 
  char path[100];        

  //test
  // if(!chmod("hihi", S_IWOTH| S_IXOTH)){
  //   printf("Workds for string\n");
  // };

  if (dp)
  {
    while ((ep = readdir (dp)) != NULL){
     if (ep->d_type == DT_DIR)
      {
        //printf("%s\n", ep->d_name);
        strcpy(path,ep->d_name);
        printf("Dir: %s\n", path);
        // directory: read + execute: 5
        //int res = chmod(path, S_IXOTH | S_IROTH);
        if( (strcmp(path,"..") != 0 ) && (strcmp(path,".") != 0 )){
          if( chmod(path , S_IROTH | S_IXOTH ) == -1 ) {
            perror(path);
          }
        }
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
    while ((ep = readdir (ds)) != NULL){
      if (ep->d_type == DT_REG)
      {
        printf("File: %s\n", ep->d_name);
        if( (strcmp(path,"..") != 0 ) && (strcmp(path,".") != 0 )){
          if( chmod(ep->d_name , S_IROTH ) == -1 ) {
            perror(path);
          }
        }
      }
    }
    closedir (ds);
  }
  else
    perror ("Couldn't open the directory");

  return 0;
}
/* Pseudo code to list file and dir

1) method to get filepath in terminal

2) To recursively give directories read&execute privileges:

find /path/to/base/dir -type d -exec chmod 755 {} +
To recursively give files read privileges:

find /path/to/base/dir -type f -exec chmod 644 {} +
Or, if there are many objects to process:

chmod 755 $(find /path/to/base/dir -type d)
chmod 644 $(find /path/to/base/dir -type f)
Or, to reduce chmod spawning:

find /path/to/base/dir -type d -print0 | xargs -0 chmod 755 
find /path/to/base/dir -type f -print0 | xargs -0 chmod 644

*/