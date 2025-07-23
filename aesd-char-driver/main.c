/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include "aesdchar.h"
#include "aesd-circular-buffer.h"
int aesd_major =   0; // use dynamic major
int aesd_minor =   0;

MODULE_AUTHOR("sbaans"); /** TODO: fill in your name: DONE **/
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

int aesd_open(struct inode *inode, struct file *filp)
{
    PDEBUG("open");
    /**
     * TODO: handle open
     */
    struct aesd_dev *dev;

	dev = container_of(inode->i_cdev, struct aesd_dev, cdev);
	filp->private_data = dev;


    return 0;
}

int aesd_release(struct inode *inode, struct file *filp)
{
    PDEBUG("release");
    /**
     * TODO: handle release
     */
    return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    uint8_t write_ptr = 0;
    uint8_t read_ptr = 0;
    size_t entry_offset_byte = 0;
    size_t remaining_entry_bytes = 0;
    ssize_t retcode = -1;
    size_t remaining_bytes_to_read = count;

    loff_t *new_offset = f_pos;

    struct aesd_buffer_entry *current_entry = NULL;

    /* Get the current read and write pointers */
    write_ptr = (aesd_device.circ_buf).in_offs;
    read_ptr = (aesd_device.circ_buf).out_offs;

    PDEBUG("read %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle read
     */

    // return all the values of the buffer , in the right order

    // If in pointer is equal to out and full is not set, then nothing to read
    if ( ( (aesd_device.circ_buf).full == false ) && ( write_ptr == read_ptr ) ){
    	PDEBUG("Nothing to read");
    	return retval;
    }

    //get the start entry
    current_entry = aesd_circular_buffer_find_entry_offset_for_fpos(&(aesd_device.circ_buf),*f_pos, &entry_offset_byte);

    // Test if it is finished
    if (current_entry == NULL) {
    	return retval;
    }

    do {
		// Remaining bytes in this entry
		remaining_entry_bytes = (current_entry->size) - (entry_offset_byte);

		if (remaining_bytes_to_read < remaining_entry_bytes ) {
			// Copy the remaining values of the entry up until count
			retcode = copy_to_user(buf, &(current_entry->buffptr)[entry_offset_byte], remaining_bytes_to_read);
			retval += remaining_bytes_to_read;
			// And it is finished
			return retval;
		} else {
			// Copy the remaining values of the entry up until the end of entry
			retcode = copy_to_user(buf, &(current_entry->buffptr)[entry_offset_byte], remaining_entry_bytes);
			retval += remaining_entry_bytes;


			// Prepare the next reading -> TO DO IN LOOP
			remaining_bytes_to_read -= remaining_entry_bytes;
			*new_offset += remaining_entry_bytes;
			current_entry = aesd_circular_buffer_find_entry_offset_for_fpos(&(aesd_device.circ_buf),*new_offset, &entry_offset_byte);
		}
	} while (current_entry != NULL);

    return retval;
}

ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = -ENOMEM;
    PDEBUG("write %zu bytes with offset %lld",count,*f_pos);
    /**
     * TODO: handle write
     */
    struct aesd_buffer_entry new_entry;
    const char *alloc_write_string;
    const char *memory_to_be_freed;
	ssize_t retcode = -1;

    // Protect with mutex
    //if (mutex_lock_interruptible(&d->lock))


    // Allocate for the new string
    alloc_write_string = kmalloc(count, GFP_KERNEL);

    // Copy the values in the allocated buffer
    retcode = copy_from_user((void *)alloc_write_string, buf, count);

    // Add the new entry with the allocated buffer
    new_entry.buffptr = alloc_write_string;
    new_entry.size=strlen(alloc_write_string);

    // Check if the buffer is full, if yes retrieve the pointer for desalloc
    if ((aesd_device.circ_buf).full == true) {
    	memory_to_be_freed = (aesd_device.circ_buf).entry[(aesd_device.circ_buf).out_offs].buffptr;
    	kfree(memory_to_be_freed);
    }

	aesd_circular_buffer_add_entry(&(aesd_device.circ_buf), &new_entry);

	// Release mutex
	//mutex_unlock(&scull_devices[i].lock);



    return retval;
}
struct file_operations aesd_fops = {
    .owner =    THIS_MODULE,
    .read =     aesd_read,
    .write =    aesd_write,
    .open =     aesd_open,
    .release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
    int err, devno = MKDEV(aesd_major, aesd_minor);

    cdev_init(&dev->cdev, &aesd_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &aesd_fops;
    err = cdev_add (&dev->cdev, devno, 1);
    if (err) {
        printk(KERN_ERR "Error %d adding aesd cdev", err);
    }
    return err;
}



int aesd_init_module(void)
{
    dev_t dev = 0;
    int result;
    result = alloc_chrdev_region(&dev, aesd_minor, 1,
            "aesdchar");
    aesd_major = MAJOR(dev);
    if (result < 0) {
        printk(KERN_WARNING "Can't get major %d\n", aesd_major);
        return result;
    }
    memset(&aesd_device,0,sizeof(struct aesd_dev));

    /**
     * TODO: initialize the AESD specific portion of the device
     */

    /* Initialize Circular Buffer */
    aesd_circular_buffer_init(&(aesd_device.circ_buf));

    result = aesd_setup_cdev(&aesd_device);

    if( result ) {
        unregister_chrdev_region(dev, 1);
    }
    return result;

}

void aesd_cleanup_module(void)
{
    dev_t devno = MKDEV(aesd_major, aesd_minor);

    cdev_del(&aesd_device.cdev);

    /**
     * TODO: cleanup AESD specific portions here as necessary
     */
    uint8_t index;
    struct aesd_buffer_entry *entry;
    AESD_CIRCULAR_BUFFER_FOREACH(entry,&(aesd_device.circ_buf),index) {
    	if (entry->buffptr != NULL) {
    		kfree(entry->buffptr);
    	}
    }


    unregister_chrdev_region(devno, 1);
}



module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
