#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>


// prepare and finalize calls for initialization and destruction of anything required
int prepare(void);

int finalize(void);

int process_arglist(int count, char **arglist);

int main(void) {
    if (prepare() != 0)
        exit(1);

    while (1) {
        char **arglist = NULL;
        char *line = NULL;
        size_t size;
        int count = 0;

        if (getline(&line, &size, stdin) == -1) {
            free(line);
            break;
        }

        arglist = (char **) malloc(sizeof(char *));
        if (arglist == NULL) {
            printf("malloc failed: %s\n", strerror(errno));
            exit(1);
        }
        arglist[0] = strtok(line, " \t\n");

        while (arglist[count] != NULL) {
            ++count;
            arglist = (char **) realloc(arglist, sizeof(char *) * (count + 1));
            if (arglist == NULL) {
                printf("realloc failed: %s\n", strerror(errno));
                exit(1);
            }

            arglist[count] = strtok(NULL, " \t\n");
        }

        if (count != 0) {
            if (!process_arglist(count, arglist)) {
                free(line);
                free(arglist);
                break;
            }
        }

        free(line);
        free(arglist);
    }

    if (finalize() != 0)
        exit(1);

    return 0;
}
