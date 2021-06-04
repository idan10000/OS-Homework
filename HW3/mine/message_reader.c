#include "message_slot.h"

#include <fcntl.h>      /* open */
#include <unistd.h>     /* exit */
#include <sys/ioctl.h>  /* ioctl */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv)
{
    int file;
    int retVal;
    char buffer[MESSAGE_SIZE];
    unsigned int channelID = atoi(argv[2]); // convert channel id argument from string to ulong

    // check if exactly the right amount of arguments inputted
    if (argc != 3) {
        fprintf(stderr, "Error, invalid number of arguments inputted: %s\n", strerror(EINVAL));
        exit(1);
    }

    // open device (file)
    file = open(argv[1], O_RDWR);
    if (file < 0) {
        fprintf(stderr, "Error, failed to open device - %s\n: %s\n", DEVICE_RANGE_NAME, strerror(errno));
        exit(1);
    }

    // change to the inputted channel, if it is 0 the code will return an error
    retVal = ioctl(file, MSG_SLOT_CHANNEL, channelID);
    if (retVal != 0) {
        fprintf(stderr, "Error, failed to execute IOCTL: %s\n", strerror(errno));
        exit(1);
    }

    // read the message saved on the channel. If no message, return an error
    retVal = read(file, buffer, MESSAGE_SIZE);
    if (retVal <= 0) {
        fprintf(stderr, "Error, failed to read channel %lu message: %s\n", channelID, strerror(errno));
        exit(1);
    }

    // output the read message to the console (stdout)
    if (write(1, buffer, retVal) != retVal) {
        fprintf(stderr, "Error, failed to output message to console: %s\n", strerror(errno));
        exit(1);
    }

    close(file);

    exit(0);
}