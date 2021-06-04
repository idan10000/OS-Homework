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
    char *message = argv[3]; // the inputted msg by the user to be saved in the channel
    int msgSize = strlen(message); // the size of the message
    unsigned int channelID = atoi(argv[2]); // convert channel id argument from string to ulong

    // check if exactly the right amount of arguments inputted
    if (argc != 4) {
        fprintf(stderr, "Error, invalid number of arguments inputted: %s\n", strerror(EINVAL));
        exit(1);
    }

    // open device (file)
    file = open(argv[1], O_RDWR);
    if (file < 0) {
        fprintf(stderr, "Error, failed to open device - %s\n: %s\n", argv[1], strerror(errno));
        exit(1);
    }

    // change to the inputted channel, if it is 0 the code will return an error
    retVal = ioctl(file, MSG_SLOT_CHANNEL, channelID);
    if (retVal != 0) {
        fprintf(stderr, "Error, failed to execute IOCTL: %s\n", strerror(errno));
        exit(1);
    }

    // write the inputted message to the buffer of the inputted channel. If no message, return an error
    retVal = write(file, message, msgSize);
    if (retVal != msgSize) {
        fprintf(stderr, "Error, failed to write message: %s\n", strerror(errno));
        exit(1);
    }

    close(file);

    exit(0);
}