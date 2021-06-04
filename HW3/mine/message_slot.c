// Declare what kind of code we want
// from the header files. Defining __KERNEL__
// and MODULE allows us to access kernel-level
// code not usually available to userspace programs.
#undef __KERNEL__
#define __KERNEL__
#undef MODULE
#define MODULE


#include <linux/kernel.h>   /* We're doing kernel work */
#include <linux/module.h>   /* Specifically, a module */
#include <linux/fs.h>       /* for register_chrdev */
#include <linux/uaccess.h>  /* for get_user and put_user */
#include <linux/string.h>   /* for memset. NOTE - not string.h!*/
#include <linux/slab.h>

MODULE_LICENSE("GPL");

//Our custom definitions of IOCTL operations
#include "message_slot.h"

static node_t* devices[256];

//================== DEVICE FUNCTIONS ===========================
static int device_open( struct inode* inode,
                        struct file*  file )
{
    unsigned long flags; // for spinlock
    printk("Invoking device_open(%p)\n", file);

    // We don't want to talk to two processes at the same time

    int minor = iminor(inode);

    info *fileInfo = kmalloc(sizeof(info), GFP_KERNEL);
    if (fileInfo == NULL) // check if failed mem allocation
        return -ENOMEM;

    fileInfo -> minor = minor;
    fileInfo -> Node = NULL;
    file -> private_data = (void*) fileInfo;

    // init channels for cur device
    if(devices[minor] == NULL) { //TODO: check what is default init
        node_t *channels;
        channels = kmalloc(sizeof(node_t), GFP_KERNEL);

        if (channels == NULL) { // check if failed mem allocation
            return -ENOMEM;
        }

    channels->id = 0;
        channels->len = 0;
        channels->next = NULL;
        channels-> message = NULL;
        node_t* test = devices[minor];
        devices[minor] = channels;
    }

    return SUCCESS;
}

//---------------------------------------------------------------


static int device_release( struct inode* inode,
                           struct file*  file)
{
    printk("Invoking device_release(%p,%p)\n", inode, file);
    // ready for our next caller
//    freeList(devices[file -> private_data -> minor]);
//    devices[file -> private_data -> minor] = NULL;
    kfree(file -> private_data);
    return SUCCESS;
}


//---------------------------------------------------------------
// a process which has already opened
// the device file attempts to read from it
static ssize_t device_read( struct file* file,
                            char __user* buffer,
                            size_t       length,
                            loff_t*      offset )
{
  // read doesnt really do anything (for now)

    printk("Invoking device_read(%p,%ld)\n", file, length);

    info* fileInfo = (info*)(file -> private_data);
    node_t* node = fileInfo -> Node;

int i;


// Check errors
    if(node == NULL){
      return -EINVAL;
    }



    if(node -> len == 0){
            return -EWOULDBLOCK;
        }

    if(length < node -> len || length == 0){
        return -ENOSPC;
    }

	for (i = 0; i < node -> len; ++i) {
    if (put_user((node -> message)[i], &buffer[i]) != 0)
            return -EIO;

	}
	return node -> len;

}

//---------------------------------------------------------------
// a processs which has already opened
// the device file attempts to write to it
static ssize_t device_write( struct file*       file,
                             const char __user* buffer,
                             size_t             length,
                             loff_t*            offset)
{
    int i;
  printk("Invoking device_write(%p,%ld)\n", file, length);

    info* fileInfo = file -> private_data;
    node_t* node = fileInfo -> Node;
    char tempBuffer[128];


  // Check errors
    if(node == NULL){
      return -EINVAL;
    }

    if(length > MESSAGE_SIZE){
      return -EMSGSIZE;
    }

    if(length == 0){
        return -EMSGSIZE;
    }

    char* mes = node->message;

  for( i = 0; i < length && i < MESSAGE_SIZE; ++i )
  {
      if(get_user(tempBuffer[i], &buffer[i]) != 0){
        return -EIO;
        }

}
    for( i = 0; i < length && i < MESSAGE_SIZE; ++i )
    {
        mes[i] = tempBuffer[i];
    }
  node -> len = i;

  // return the number of input characters used
  return i;
}

//----------------------------------------------------------------
static long device_ioctl( struct   file* file,
                          unsigned int   ioctl_command_id,
                          unsigned long  ioctl_param )
{
    printk( "Invoking ioctl\n");
    printk("MSG_SLOT_CHANNEL = %d\n", MSG_SLOT_CHANNEL);
    printk("ioct_command_id = %d\n", ioctl_command_id);


    // Handle the IOCTL command
    if(MSG_SLOT_CHANNEL == ioctl_command_id )
    {
        // Get the parameter given to ioctl by the process
        printk( "Entered MSG command\n");

        if(ioctl_param == 0){ // check if illegal channel
            return -EINVAL;
        }
        info* fileInfo = (info*) (file -> private_data);
        node_t* runningNode;
        runningNode = devices[fileInfo->minor];
        printk("ioctl received minor %d\n", fileInfo->minor);
        int foundNode = 0;

        while (runningNode -> next != NULL) {
            if(runningNode->id == ioctl_param){
                fileInfo -> Node = runningNode;
                foundNode = 1;
                break;
            }
            runningNode = runningNode->next;
        }
        if(runningNode->id == ioctl_param){
            fileInfo -> Node = runningNode;
            foundNode = 1;
        }
        if(!foundNode){
            node_t* tempNode = (node_t *) kmalloc(sizeof(node_t),GFP_KERNEL);
            if(tempNode == NULL){
                return -ENOMEM;
            }
            printk("ioctl creating node\n");
            tempNode -> id = ioctl_param;
            tempNode -> next = NULL;
            tempNode -> len = 0;
            tempNode -> message = kmalloc(MESSAGE_SIZE, GFP_KERNEL);
            runningNode -> next = tempNode;
            fileInfo->Node = tempNode;
        }
        printk("ioctl - len = %d\n",fileInfo -> Node -> len);
        return SUCCESS;
    }
    printk("ioctl - wrong command\n");

    // If not the correct command return error
    return -EINVAL;

}

//==================== DEVICE SETUP =============================

// This structure will hold the functions to be called
// when a process does something to the device we created
struct file_operations Fops =
        {
                .owner	  = THIS_MODULE,
                .read           = device_read,
                .write          = device_write,
                .open           = device_open,
                .unlocked_ioctl = device_ioctl,
                .release        = device_release,
        };

//---------------------------------------------------------------
// Initialize the module - Register the character device
static int __init simple_init(void)
{
    int rc = -1;
    // Register driver capabilities. Obtain major num
    rc = register_chrdev( MAJOR_NUM, DEVICE_RANGE_NAME, &Fops );

    // Negative values signify an error
    if( rc < 0 )
    {
        printk( KERN_ERR "%s registration failed for  %d\n",
                DEVICE_FILE_NAME, MAJOR_NUM );
        return rc;
    }

    // init once the device array
//    devices = (node_t **) kmalloc(sizeof(node_t *) * 256, GFP_KERNEL);
//    if (devices == NULL) {
//        return -ENOMEM;
//    }

    int i;
    for (i = 0; i < 256; ++i) {
        devices[i] = NULL;
    }


    printk( "Registration is successful. ");
    printk( "If you want to talk to the device driver,\n" );
    printk( "you have to create a device file:\n" );
    printk( "mknod /dev/%s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM );
    printk( "You can echo/cat to/from the device file.\n" );
    printk( "Dont forget to rm the device file and "
            "rmmod when you're done\n" );

    return 0;
}

//---------------------------------------------------------------

void freeList(struct node* head)
{
    struct node* tmp;

    while (head != NULL)
    {
        tmp = head;
        head = head->next;
        if(tmp->message != NULL)
            kfree(tmp->message);
        kfree(tmp);
    }
}


static void __exit simple_cleanup(void)
{
    // Unregister the device
    // Should always succeed
    int i;
    for (i = 0; i < 256; ++i) {
        //printk("deleting device index: %d",i);
        if(devices[i] != NULL)
            freeList(devices[i]);
    }
//    kfree(devices);
    unregister_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME);
}

//---------------------------------------------------------------
module_init(simple_init);
module_exit(simple_cleanup);

//========================= END OF FILE =========================
