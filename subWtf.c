#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main ()
{
  char *str = (char*) malloc(100*sizeof(char));
  strcpy(str,"./test/tool.c");
  char res[50];
  char * pch;
  printf ("Splitting string \"%s\" into tokens:\n",str);
  pch = strtok (str,"/");
  while (pch != NULL)
  {
  	strcpy(res,pch);
    printf ("Res is: %s\n",res);
    pch = strtok (NULL, "/");
  }

  printf ("Res is: %s\n",str);

  return 0;
}