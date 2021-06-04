#include "message_slot.h"

#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>  /* ioctl */
#include <errno.h>
#include <assert.h>
#include <sys/wait.h>

int main(int argc, char **argv){
    int fd1, fd2, rc, pid;
    int exit_code = -1;
    int value = 123;
    char buff[12];
    char buff2[4];
    char *arguments1[] = {"./start.sh", NULL};
    char *arguments2[] = {"./end.sh", NULL};
    
    printf("\n**************\n     start    \n**************\n");

    pid = fork();
    if(pid < 0){
        perror("Error forking: ");
        exit(1);
    } else if( pid == 0){
        if(execvp(arguments1[0],arguments1) < 0){
            perror("Error:");
            exit(1);
        }
    }
    waitpid(pid,&exit_code,0);
    printf("Done loading module\n");
    printf("Opening none existing file...");
    fd1 = open("/dev/slot2",O_RDWR);
    assert(fd1 < 0);
    printf("Success\n");
    printf("Opening existing file...");
    fd1 = open("/dev/slot0",O_RDWR);
    assert(fd1 > 0);
    fd2 = open("/dev/slot1",O_RDWR);
    assert(fd2 > 0);
    printf("Success\n");
    printf("writing and reading before ioctl...");
    rc = write(fd1,"hello",6);
    assert(rc < 0 && errno == EINVAL);
    rc = read(fd1,&buff,6);
    assert(rc < 0 && errno == EINVAL);
    printf("Success\n");
    printf("wrong command...");
    rc = ioctl(fd1,0,5);
    assert(rc < 0 && errno == EINVAL);
    printf("Success\n");
    printf("wrond channel id...");
    rc = ioctl(fd1,MSG_SLOT_CHANNEL,0);
    assert(rc < 0 && errno == EINVAL);
    printf("Success\n");
    printf("change channel id to 20...");
    rc = ioctl(fd1,MSG_SLOT_CHANNEL,20);
    assert(rc == 0);
    printf("Success\n");
    printf("writing HELLO WORLD...");
    rc = write(fd1,"HELLO WORLD",12);
    assert(rc > 0);
    printf("Success\n");
    printf("change channel id to 5...");
    rc = ioctl(fd1,MSG_SLOT_CHANNEL,5);
    assert(rc == 0);
    printf("Success\n");
    printf("reading from empty channel...");
    rc = read(fd1,&buff,6);
    assert(rc < 0 && errno == EWOULDBLOCK);
    printf("Success\n");
    printf("wrong size buffer values...");
    rc = write(fd1,"hello",0);
    assert(rc < 0 && errno == EMSGSIZE);
    rc = write(fd1,"hello",129);
    assert(rc < 0 && errno == EMSGSIZE);
    printf("Success\n");
    printf("writing hello to the channel...");
    rc = write(fd1,"hello",6);
    assert(rc > 0);
    printf("Success\n");
    printf("reading size 0...");
    rc = read(fd1,&buff,0);
    assert(rc < 0 && errno == ENOSPC);
    printf("Success\n");
    printf("reading with buffer size to small...");
    rc = read(fd1,&buff,5);
    assert(rc < 0 && errno == ENOSPC);
    printf("Success\n");
    printf("reading hello message...");
    rc = read(fd1,&buff,6);
    assert(rc > 0);
    assert(strcmp(buff,"hello") == 0);
    printf("Success\n");
    printf("change channel and read old message at channel 20...");
    rc = ioctl(fd1,MSG_SLOT_CHANNEL,20);
    assert(rc == 0);
    rc = read(fd1,&buff,12);
    assert(rc > 0);
    assert(strcmp(buff,"HELLO WORLD") == 0);
    printf("Success\n");
    printf("change channel on second file and read from the first file...");
    rc = ioctl(fd2,MSG_SLOT_CHANNEL,10);
    assert(rc == 0);
    rc = read(fd1,&buff,12);
    assert(rc > 0);
    assert(strcmp(buff,"HELLO WORLD") == 0);
    printf("Success\n");
    printf("reading from the second file...");
    rc = read(fd2,&buff,6);
    assert(rc < 0 && errno == EWOULDBLOCK);
    printf("Success\n");
    printf("writing and reading int value to channel...");
    rc = write(fd1,&value,4);
    assert(rc > 0);
    rc = read(fd1,&buff2,4);
    assert(rc > 0);
    assert( (int) *buff2 == 123);
    printf("Success\n");
    printf("check if channel can be bigger than 2^1024...");
    rc = ioctl(fd1,MSG_SLOT_CHANNEL,4000000000);
    assert(rc == 0);
    rc = write(fd1,"long num",9);
    assert(rc > 0);
    rc = read(fd1,&buff,9);
    assert(rc > 0);
    assert(strcmp(buff,"long num") == 0);  
    printf("Success\n");
    assert(close(fd1) == 0);
    assert(close(fd2) == 0);
    printf("Start cleaning module\n");
    pid = fork();
    if(pid < 0){
        perror("Error forking: ");
        exit(1);
    } else if( pid == 0){
        if(execvp(arguments2[0],arguments2) < 0){
            perror("Error:");
            exit(1);
        }
    }
    waitpid(pid,&exit_code,0);
    printf("\n**************\n     Done    \n**************\n");
    return 0;
}
