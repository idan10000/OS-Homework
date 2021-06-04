//
// Created by idanp on 10-May-21.
//

#ifndef HW3_MESSAGE_SLOT_H
#define HW3_MESSAGE_SLOT_H

#include <linux/ioctl.h>

// The major device number.
// We don't rely on dynamic registration
// any more. We want ioctls to know this
// number at compile time.
//#define MAJOR_NUM 244
#define MAJOR_NUM 240

// Set the message of the device driver
#define MSG_SLOT_CHANNEL _IOW(MAJOR_NUM, 0, unsigned int)
#define MESSAGE_SIZE 128

#define DEVICE_RANGE_NAME "message_slot"


#define DEVICE_FILE_NAME "message_slot_dev"
#define SUCCESS 0

typedef struct node {
    char* message;
    int len;
    unsigned long id;
    struct node * next;
} node_t;

typedef struct device_info
{
    int minor;
    node_t* Node;
} info;

#endif //HW3_MESSAGE_SLOT_H
