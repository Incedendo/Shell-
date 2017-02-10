#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

int main(void) {
  DIR *dh = opendir("./"); // directory handle
  struct dirent *file; // a 'directory entity' AKA file    
  struct stat info; // info about the file.
  while (file = readdir(dh)) {
    if (stat(file->d_name, &info))
    {
        printf("error: stat(%s): %s\n", file->d_name, strerror(errno));
        continue;
    }
    printf("note: file->d_name => %s\n", file->d_name);
    printf("note: info.st_mode => %i\n", info.st_mode);
    if (S_ISREG(info.st_mode))
      printf("REGULAR FILE FOUND! %s\n", file->d_name);
    }
  closedir(dh);

  return 0;
}