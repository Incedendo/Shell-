#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

//#include <sys/types.h>
#include <sys/stat.h> 

int main (void)
{
  DIR *dp;
  struct dirent *ep;     
  dp = opendir ("./");
  char path[PATH_MAX];

  if (dp)
  {
    while ((ep = readdir (dp)) != NULL){
      if (ep->d_type == DT_DIR)
      {
       printf("%s\n", ep->d_name);
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