//mostly taken from recitation
#ifndef _MESSAGE_SLOT_H_
#define _MESSAGE_SLOT_H_

#include <linux/ioctl.h>

#define MAJOR_NUM 240

// Set the message of the device driver
#define MSG_SLOT_CHANNEL _IOW(MAJOR_NUM, 0, unsigned long)

#define DEVICE_RANGE_NAME "message_slot"
#define BUF_LEN 128
#define SUCCESS 0

typedef struct myMessage {
	char *data;
	int size;
} message;

typedef struct myChannel {
	long id;
	struct message *msg;
	struct myChannel *next;
} channel;


typedef struct mySlot {
	int minor;
	struct channel *head;
	struct channel *tail;
	struct channel *current_channel;
} slot;


#endif
