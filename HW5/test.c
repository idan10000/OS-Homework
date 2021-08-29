//
// Created by idanp on 01-Jul-21.
//
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

int main(int argc, char *argv[])
{
    int pipefd[2];
    pid_t cpid;
    char buf;
    char* msg = "abcd";
    int msg_len = 5;
    pipe(pipefd);
    cpid = fork();
    if( cpid == 0 )
    {
        write(pipefd[1], msg, msg_len);
        while( read(pipefd[0], &buf, 1) > 0 )
        write( STDOUT_FILENO, &buf, 1);
        write( STDOUT_FILENO, "\n", 1);
        close( pipefd[0] );
        exit(EXIT_SUCCESS);
    }
    else
    {
        close(pipefd[0]);
        write(pipefd[1], msg, msg_len);
        close(pipefd[1]);
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}