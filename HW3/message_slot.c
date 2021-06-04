//the whole code in this file is taken from recitation 6 and chenged on if needed

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

#include "message_slot.h"


// used to prevent concurent access into the same device
static int dev_open_flag = 0;



//Do we need to encrypt?
static int encryption_flag = 0;

slot* arr[257];

//helping functions

slot* create_slot(int minor){
    slot* new_slot = kmalloc(sizeof(slot), GFP_KERNEL);
    if(new_slot == NULL){
      printk("error in kmalloc\n");
      return -ENOMEM;
    }
    new_slot->minor= minor;
    new_slot->head = NULL;
    new_slot->tail = NULL;
    new_slot->current_channel= NULL;
    return new_slot;
}

message* create_message(void){
	message* new_message = kmalloc(sizeof(message), GFP_KERNEL);
	if(new_message == NULL){
      printk("error in kmalloc\n");
      return -ENOMEM;
    }
	new_message->data = kmalloc(sizeof(BUF_LEN), GFP_KERNEL);
    if(new_message->data == NULL){
        printk("error in kmalloc\n");
        return -ENOMEM;
    }
    new_message->data=NULL;
    new_message->size=0;
    return new_message;
}

channel* create_channel(long id){
    channel* new_channel = kmalloc(sizeof(channel), GFP_KERNEL);
    if(new_channel == NULL){
      printk("error in kmalloc\n");
      return -ENOMEM;
    }
    new_channel->id= id;
    new_channel->msg = create_message();
    new_channel->next = NULL;
    return new_channel;
}



static int device_open( struct inode* inode,
                        struct file*  file )
{
  int minor = iminor(inode);

  printk("Invoking device_open(%p)\n", file);


  if (arr[minor]==NULL){
  	slot new_slot=create_slot(minor);
  	arr[minor]=new_slot;
  	file->private_data=(void*)new_slot;
  }
  else{
  	file->private_data=(void*)arr[minor];
  }

  ++dev_open_flag;
  return SUCCESS;
}


static int device_release( struct inode* inode,
                           struct file*  file)
{
  printk("Invoking device_release(%p,%p)\n", inode, file);

  kfree(file->private_data);

  // ready for our next caller
  --dev_open_flag;
  return SUCCESS;
}


static long device_ioctl( struct   file* file,
                          unsigned int   ioctl_command_id,
                          unsigned long  ioctl_param )
{
  // Switch according to the ioctl called
  if( IOCTL_SET_ENC == ioctl_command_id )
  {
    // Get the parameter given to ioctl by the process
    printk( "Invoking ioctl: setting encryption "
            "flag to %ld\n", ioctl_param );
    encryption_flag = ioctl_param;
  }
  if (ioctl_command_id!=MSG_SLOT_CHANNEL || ioctl_param == 0){
  		return -EINVAL;
  }

  slot* slt = (slot*)(file->private_data);
  channel chnl;
  channel tmp;
  channel chnl;
  if (slt->head==NULL){ //first channel
  	chnl = create_channel(ioctl_param);
  	slt->head=chnl;
  	slt->tail=chnl;
  	slt->current_channel=chnl;
  }
  else{
  		tmp = slt->head;
  		prev=NULL;
		while (tmp != NULL) {
			if (tmp->id== ioctl_param) { //search if channel exists
				slt->current_channel = tmp;
				return SUCCESS;
			}
			prev=tmp;
			tmp = tmp->next;
		}

		prev->next=create_channel(ioctl_param);
		slt->current_channel = prev->next;
		slt->tail=prev->next;

  }


  return SUCCESS;
}



static ssize_t device_read( struct file* file,
                            char __user* buffer,
                            size_t       length,
                            loff_t*      offset )
{
  printk( "Invocing device_read(%p,%ld) - "
          file, length);

  slot* slt = (slot*)(file->private_data);
  if (slt->curr_channel==NULL){
  	printk("no channel has been set on the file descriptor");
  	return -EINVAL;
  }

  if(((slt->curr_channel)->message)->size == 0){
    printk("no message to be read");
    return -EWOULDBLOCK;
  }

    if((curr_channel->message)->size > length){
    printk("buffer length is too small to hold the last message written on the channel");
    return -ENOSPC;
  }
 //taken from:  https://www.kernel.org/doc/htmldocs/kernel-api/API---copy-to-user.html
    if(copy_to_user(buffer, ((slt->curr_channel)->message)->data, ((slt->curr_channel)->message)->size))!=0{
        return -EFAULT;
    }

  return ((slt->curr_channel)->message)->size;
}


static ssize_t device_write( struct file*       file,
                             const char __user* buffer,
                             size_t             length,
                             loff_t*            offset)
{
  printk("Invoking device_write(%p,%ld)\n", file, length);

  char buf[length];

  slot* slt = (slot*)(file->private_data);
  if (slt->curr_channel==NULL){
  	printk("no channel has been set on the file descriptor");
  	return -EINVAL;
  }

  if (length==0 || length>BUF_LEN){
  	printk("error in message size");
  	return -EMSGSIZE;
  }

//taken from https://www.cs.bham.ac.uk/~exr/teaching/lectures/opsys/13_14/docs/kernelAPI/r4081.html
  if (copy_from_user(((slt->curr_channel)->message)->data, buffer, length) != 0){
  	((slt->curr_channel)->message)->data=NULL;
  	return -EFAULT;
  }
 

  return i;
}


struct file_operations Fops =
{
  .owner	  = THIS_MODULE, 
  .read           = device_read,
  .write          = device_write,
  .open           = device_open,
  .unlocked_ioctl = device_ioctl,
  .release        = device_release,
};

static int __init simple_init(void)
{
  int rc = -1;
  // init dev struct

  // Register driver capabilities. Obtain major num
  rc = register_chrdev( MAJOR_NUM, DEVICE_RANGE_NAME, &Fops );

  // Negative values signify an error
  if( rc < 0 )
  {
    printk( KERN_ALERT "%s registraion failed for  %d\n",
                       DEVICE_RANGE_NAME, MAJOR_NUM );
    return rc;
  }

  return 0;
}

static void __exit simple_cleanup(void)
{
  // Unregister the device
  // Should always succeed
	channel* chnl;
	channel* next;


	for (int i=0; i<257; i++){
	if (arr[i]!=NULL){
	chnl = arr[i]->head;
        while(chnl != NULL){
          next=chnl->next;
          kfree(chnl);
          chnl = next;
        }
       kfree(arr[i]);
		}
	}
  

  unregister_chrdev(MAJOR_NUM, DEVICE_RANGE_NAME);
}


module_init(simple_init);
module_exit(simple_cleanup);



