#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/resource.h>


sigset_t sigset;


// prepare and finalize calls for initialization and destruction of anything required
int prepare(void) {

    // setup sigset to block SIGINT for main process
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGINT);

    // using SA_NOCLDWAIT so when a parent doesn't wait for a child it does not become a zombie
    struct sigaction sigCHLDIgnore;
    sigCHLDIgnore.sa_flags = SA_NOCLDWAIT;

    if (sigaction(SIGCHLD, &sigCHLDIgnore, 0) != 0) {
        fprintf(stderr, "Signal handle registration " "failed. %s\n", strerror(errno));
        return -1;
    }

    // block SIGINT
    sigprocmask(SIG_BLOCK, &sigset, NULL);


    return 0;
}

int finalize(void) {
    return 0;
}

// Goes over arglist and checks if it contains the arg (| or >). If it does returns its index, otherwise returns 0
int containsSpecialArg(char **arglist, char *arg) {
    int i = 0;
    while (arglist[i] != NULL) {
        if (strcmp(arglist[i], arg) == 0) {
            return i;
        }
        i++;
    }
    return 0;
}

// Handles running background command
int backgroundHandling(int count, char **arglist) {
    int pid = fork();
    if (pid == 0) {
        int pid1 = fork();
        if (pid1 == 0) {
            arglist[count - 1] = NULL;
            execvp(arglist[0], arglist);
            fprintf(stderr, "Error at background execvp: %s\n", strerror(errno));
            exit(1);
        }
        if (pid1 < 0) {
            fprintf(stderr, "Error at background inner fork: %s\n", strerror(errno));
            return 0;
        }
        int wRes, status;
        wRes = waitpid(pid1, &status, 0);
        if (wRes == -1 && errno != EINTR && errno != ECHILD) {
            fprintf(stderr, "Error at background wait: %s\n", strerror(errno));
            exit(1);
        }
        exit(0);
    }
    if (pid < 0) {
        fprintf(stderr, "Error at background fork: %s\n", strerror(errno));
        return 0;
    }
    return 1;
}

// Handles command with pipe argument
int pipeHandling(int count, char **arglist, int pipeIndex) {
    int pfd[2];
    int status, wRes;

    // open pipe
    if (pipe(pfd) == -1) {
        fprintf(stderr, "Error creating pipe: %s\n", strerror(errno));
        exit(1);
    }

    int readerfd = pfd[0];
    int writerfd = pfd[1];
    arglist[pipeIndex] = NULL;

    int pid1 = fork();
    if (pid1 == 0) {
        // first pipe command

        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
        close(readerfd); // close reading end before writing
        if (dup2(writerfd, 1) == -1) { // switch output from stdout to write pipe
            fprintf(stderr, "Error at dup2 pipe first func child: %s\n", strerror(errno));
            exit(1);
        }
        close(writerfd);

        execvp(arglist[0], arglist);
        fprintf(stderr, "Error at pipe first execvp: %s\n", strerror(errno));
        exit(1);

    }
    if (pid1 < 0) {
        fprintf(stderr, "Error at pipe first fork: %s\n", strerror(errno));
        return 0;
    }


    int pid2 = fork();

    if (pid2 == 0) {
        // second pipe command
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);

        close(writerfd); // close reading end before writing
        if (dup2(readerfd, 0) == -1) { // switch input from stdin to read pipe
            fprintf(stderr, "Error at dup2 pipe second func child: %s\n", strerror(errno));
            exit(1);
        }
        close(readerfd);

        // execute second command of the pipe, which is at index [pipeIndex + 1] and pass it only its arguments
        execvp(arglist[pipeIndex + 1], arglist + pipeIndex + 1);
        fprintf(stderr, "Error at pipe first execvp: %s\n", strerror(errno));
        exit(1);

    }
    if (pid2 < 0) {
        fprintf(stderr, "Error at pipe second fork: %s\n", strerror(errno));
        return 0;
    }

    // close pipe
    close(writerfd);
    close(readerfd);

    //wait for children to finish

    wRes = waitpid(pid1, &status, 0);
    if (wRes == -1 && errno != EINTR && errno != ECHILD) {
        fprintf(stderr, "Error at first wait: %s\n", strerror(errno));
        return 0;
    }

    wRes = waitpid(pid2, &status, 0);
    if (wRes == -1 && errno != EINTR && errno != ECHILD) {
        fprintf(stderr, "Error at first wait: %s\n", strerror(errno));
        return 0;
    }
    return 1;

}

// Handles redirecting output of command to file
int outputRedirectHandling(int count, char **arglist, int redirectIndex) {
    int status, wRes;

    arglist[redirectIndex] = NULL;

    int pid = fork();
    if (pid == 0) {
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
        // open file, has create flag to create one if it does not exist
        int fd = open(arglist[redirectIndex + 1], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd < 0) {
            fprintf(stderr, "Error opening file at redirect child: %s\n", strerror(errno));
            exit(1);
        }

        // redirect output to file
        if (dup2(fd, 1) == -1) {
            fprintf(stderr, "Error at dup2 redirect func child: %s\n", strerror(errno));
            exit(1);
        }


        execvp(arglist[0], arglist);
        fprintf(stderr, "Error at redirect execvp: %s\n", strerror(errno));
        exit(1);
    }

    if (pid < 0) {
        fprintf(stderr, "Error at redirect fork: %s\n", strerror(errno));
        return 0;
    }

    wRes = waitpid(pid, &status, 0);
    if (wRes == -1 && errno != EINTR && errno != ECHILD) {
        fprintf(stderr, "Error at redirect wait: %s\n", strerror(errno));
        return 0;
    }
    return 1;
}

// arglist - a list of char* arguments (words) provided by the user
// it contains count+1 items, where the last item (arglist[count]) and *only* the last is NULL
// RETURNS - 1 if should continue, 0 otherwise
int process_arglist(int count, char **arglist) {
    // run background child

    sigprocmask(SIG_BLOCK, &sigset, NULL);

    // handle background run
    if (strcmp(arglist[count - 1], "&") == 0) {
        return backgroundHandling(count, arglist);
    }

    // handle pipe
    int pipeIndex = containsSpecialArg(arglist,
                                       "|"); // returns the index of the | symbol or 0 if there isn't one
    if (pipeIndex != 0) {
        return pipeHandling(count, arglist, pipeIndex);
    }

    // handle redirect command
    int redirectIndex = containsSpecialArg(arglist, ">");
    if (redirectIndex != 0) {
        return outputRedirectHandling(count, arglist, redirectIndex);
    }

    // normal command handling

    int pid = fork();
    if (pid == 0) {
        sigprocmask(SIG_UNBLOCK, &sigset, NULL);
        execvp(arglist[0], arglist);
        fprintf(stderr, "Error at process_arglist execvp: %s\n", strerror(errno));
        exit(1);
    }
    if (pid < 0) {
        fprintf(stderr, "Error at process_arglist fork: %s\n", strerror(errno));
        return 0;
    }
    int status, wRes;
    wRes = waitpid(pid, &status, 0);
    if (wRes == -1 && errno != EINTR && errno != ECHILD) {
        fprintf(stderr, "Error at process_arglist wait: %s\n", strerror(errno));
        return 0;
    }

    return 1;


}
