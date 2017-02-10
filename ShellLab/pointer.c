#include <stdio.h>
#include <stdlib.h>

typedef struct Katze {
    int value;
} Katze;

int main () {
    Katze *mew = (Katze*) malloc(sizeof(Katze));
    mew->value = 8910;
    free(mew);
    //mew = NULL;
    printf("mew: %d\n", mew->value); // why does this still say "mew: 8910"?

    int *ptr = (int *) malloc(sizeof(int));
	*ptr = 5;
	int a = *ptr;
	printf("a = %d\n",a);
	/* Do some work */

	free(ptr);
	//ptr = NULL;
	int b = *ptr;
	printf("b = %d\n",b);

	return 0;
}