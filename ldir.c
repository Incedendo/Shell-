#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
//#include <sys/types.h>
#include <sys/stat.h> 

int main (void)
{
  DIR *dp;
  struct dirent *ep;     
  dp = opendir ("./");


  // char file[100] = "a";
  // if( chmod(file, S_IRGRP ) == -1 ) {
  //       perror(file);
  // }

  char file[100];

  if (dp)
  {
    while ((ep = readdir (dp)) != NULL){
      if (ep->d_type == DT_DIR)
      {
        strcpy(file, ep->d_name);
        printf("%s has size %lu\n", ep->d_name, strlen(ep->d_name));
        printf("%s has size %lu\n\n", file, strlen(file));

        // directory: read + execute: 5
        //int res = chmod(path, S_IXOTH | S_IROTH);

       
        //printf("file name: %s\n", file);
        if( chmod(file, S_IRGRP ) == -1 ) {
          perror(ep->d_name);        
        }else{
          printf("successfully change mode of %s to \n", file);
        }
      }
    }
      

    closedir (dp);
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