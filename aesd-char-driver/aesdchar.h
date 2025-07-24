/*
 * aesdchar.h
 *
 *  Created on: Oct 23, 2019
 *      Author: Dan Walkes
 */

#ifndef AESD_CHAR_DRIVER_AESDCHAR_H_
#define AESD_CHAR_DRIVER_AESDCHAR_H_

#include "aesd-circular-buffer.h"

#define AESD_DEBUG 1  //Remove comment on this line to enable debug

#define WAITING_BUFFER_SIZE 50

#undef PDEBUG             /* undef it, just in case */
#ifdef AESD_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "aesdchar: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

struct aesd_dev
{
    /**
     * TODO: Add structure(s) and locks needed to complete assignment requirements
     */
    struct cdev cdev;     /* Char device structure      */

    /* Circular Buffer */
    struct aesd_circular_buffer circ_buf;

    /* Waiting Buffer */
    const char waiting_bfr[WAITING_BUFFER_SIZE];

    /* Waiting Buffer offset */
    uint8_t waiting_bfr_offset;

    /* Mutex */
    struct mutex lock;     /* mutual exclusion semaphore     */
};

int aesd_open(struct inode *inode, struct file *filp);
int aesd_release(struct inode *inode, struct file *filp);
ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos);
ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos);
int aesd_init_module(void);
void aesd_cleanup_module(void);


#endif /* AESD_CHAR_DRIVER_AESDCHAR_H_ */
