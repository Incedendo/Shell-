#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CMD_LEN  128
#define HISTORY_COUNT 10

int history(char *hist[], int current, int actual)
{
        int i = current;

        int hist_num = 1;
        if(actual > 10)
                hist_num = actual - 9;

        do {
                if (hist[i]) {
                        printf("%4d  %s\n", hist_num, hist[i]);
                        hist_num++;
                }

                i = (i + 1) % HISTORY_COUNT;

        } while (i != current);

        return 0;
}

int clear_history(char *hist[])
{
        int i;

        for (i = 0; i < HISTORY_COUNT; i++) {
                free(hist[i]);
                hist[i] = NULL; //good programming habit to set pointer to NULL
                //after freeing it.
        }

        return 0;
}

int main()
{
        char cmd[MAX_CMD_LEN];
        char *hist[HISTORY_COUNT];
        int i, current,actual = 0;

        for (i = 0; i < HISTORY_COUNT; i++)
                hist[i] = NULL;

        while (1) {
                printf("user@shell # ");

                fgets(cmd, MAX_CMD_LEN, stdin);

                /* get rid of the '\n' from fgets */
                if (cmd[strlen(cmd) - 1] == '\n')
                        cmd[strlen(cmd) - 1] = '\0';

                /* NULL to `free` is a NO-OP */
                free(hist[current]);

                hist[current] = strdup(cmd);

                current = (current + 1) % HISTORY_COUNT;
                actual = actual + 1;

                if (!strcmp(cmd, "history"))
                        history(hist, current,actual);
                else if (!strcmp(cmd, "hc"))
                        clear_history(hist);
                else if (!strcmp(cmd, "quit"))
                        break;
                else if(!strcmp(cmd, "!!")){
                        strcpy(hist[current-1], hist[current-2]);
                        //print the most recent command in hist
                        printf("Most current cm before !!: %4d  %s\n", current-1, hist[current-2]);
                }else if(cmd[0] == '!'){
                        char number = cmd[strlen(cmd)-1];
                        int x = number - '0';
                        if(x == 0) x = 10;
                        strcpy(hist[current-1], hist[x-1]);
                        printf("Re-execute Command number %d which is %s\n",x, hist[x-1]);
                }
        }

        clear_history(hist);

        return 0;
}