#define  _GNU_SOURCE
#define  _POSIX_C_SOURCE 200809L

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    char   *input_arg[2];
    char   *input_str = NULL;
    size_t  input_len = 0;
    char  **args;
    ssize_t len;
    size_t  n;
    pid_t   child, p;
    int     status;

    if (argc < 2) {

        /* No command line parameters. Read command from stdin. */
        len = getline(&input_str, &input_len, stdin);

        /* Find length excluding the newline at end. */
        if (len > (ssize_t)0)
            n = strcspn(input_str, "\r\n");
        else
            n = 0;

        if (n > (size_t)0) {

            /* Terminate input command before the newline. */
            input_str[n] = '\0';

        } else {

            fprintf(stderr, "No input, no command.\n");
            return 1;
        }

        input_arg[0] = input_str;
        input_arg[1] = NULL;
        args = input_arg;

    } else {
        /* Use command line parameters */
        argv[argc] = NULL;
        args = argv + 1;
    }

    child = fork();
    if (child == (pid_t)-1) {
        fprintf(stderr, "Cannot fork: %s.\n", strerror(errno));
        return 1;
    }

    if (!child) {
        /* This is the child process. */

        errno = ENOENT;
        execvp(args[0], args);

        fprintf(stderr, "%s: %s.\n", args[0], strerror(errno));
        exit(127);
    }

    do {
        p = waitpid(child, &status, 0);
    } while (p == (pid_t)-1 && errno == EINTR);
    
    if (p == (pid_t)-1) {
        fprintf(stderr, "Lost child process: %s.\n", strerror(errno));
        return 127;
    }
    if (p != child) {
        fprintf(stderr, "waitpid() library bug occurred.\n");
        return 127;
    }

    if (WIFEXITED(status)) {
       if (!WEXITSTATUS(status))
           fprintf(stderr, "Command successful.\n");
       else
           fprintf(stderr, "Command failed with exit status %d.\n", WEXITSTATUS(status));
       return WEXITSTATUS(status);
    }

    if (WIFSIGNALED(status)) {
        fprintf(stderr, "Command died by signal %s.\n", strsignal(WTERMSIG(status)));
        return 126;
    }

    fprintf(stderr, "Command died from unknown causes.\n");
    return 125;
}