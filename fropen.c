/* freopen example: redirecting stdout */
#include <stdio.h>

int main (int argc, char **argv)
{

	if (argc != 2) {
		fprintf(stderr, "usage: %s output_file\n", argv[0]);
		exit(1);
	}

  	freopen (argv[1],"w",stdout);
  	printf ("This test is redirected to %s.", argv[1]);
  	fclose (stdout);
  	return 0;
}