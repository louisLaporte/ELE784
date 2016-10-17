#include "scull.h"

struct file_operations scull_fops = {
	.owner	        =	THIS_MODULE,
	.read		=	scull_read,
	.write	        =	scull_write,
	.open		=	scull_open,
	.release	=	scull_release
};

int buf_in(struct ring_buffer *buf, unsigned char *data) {
  if (buf->buf_full)
    return -1;
  buf->buf_empty = 0;
  buf->buffer[buf->in_idx] = *data;
  buf->in_idx = (buf->in_idx + 1) % buf->buf_size;
  if (buf->in_idx == buf->out_idx)
    buf->buf_full = 1;
  return 0;
}

int buf_out(struct ring_buffer *buf, unsigned char *data) {
        if (buf->buf_empty)
                return -1;
        buf->buf_full = 0;
        *data = buf->buffer[buf->out_idx];
        buf->out_idx = (buf->out_idx + 1) % buf->buf_size;
        if (buf->out_idx == buf->in_idx)
                buf->buf_empty = 1;
        return 0;
}


int scull_open(struct inode *inode, struct file *filp) {

        printk(KERN_WARNING"scull_open\n");

        switch ((filp->f_flags & O_ACCMODE)) {
        case O_RDONLY:
                printk("Read only\n");
                scull_dev.num_reader++;
                break;
        case O_WRONLY:
                printk("Write only\n");
                if(scull_dev.num_writer > 0)
                        return -ENOTTY; /* Not a typewriter */
                scull_dev.num_writer = MAX_WRITER;
                break;
        case O_RDWR:
                printk("Read and write\n");
                if(scull_dev.num_writer > 0)
                        return -ENOTTY; /* Not a typewriter */
                scull_dev.num_writer = MAX_WRITER;
                break;
        default:
                break;
        }
        printk(KERN_WARNING"nb: reader = %d | writer = %d ", 
                        scull_dev.num_reader, scull_dev.num_writer);
        return 0;
}

int scull_release(struct inode *inode, struct file *filp) {
        
        printk(KERN_WARNING"scull_release\n");

        switch(filp->f_flags & O_ACCMODE) {
        case O_RDONLY:
                scull_dev.num_reader--;
                break;
        case O_WRONLY:
                scull_dev.num_writer--;
                break;
        case O_RDWR:
                scull_dev.num_writer--;
                break;
        default:
                break;
        }
        printk(KERN_WARNING"nb: reader = %d | writer = %d ", 
                        scull_dev.num_reader, scull_dev.num_writer);
        return 0;
}

static ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos) 
{
        ssize_t result = 0;
        char ch;

        if (num > 0) {
                ch = tampon[--num];
                /*
                 * Copy data from kernel space to user space.
                 * Returns number of bytes that could not be copied.
                 * On success, this will be zero.
                 */
                if(copy_to_user(buf, &ch, 1)) {
                        result = -EFAULT; /* Bad address */
                        goto out;
                }

                printk(KERN_WARNING"scull_read (%s:%u) count = %lu ch = %c\n",
                        __FUNCTION__, __LINE__, count, ch);
                return 1;
        } else {
                printk(KERN_WARNING"scull_read (%s:%u) count = %lu ch = no char\n",
                        __FUNCTION__, __LINE__, count);
                return 0;
        }
out:
        return result;

}

static ssize_t scull_write(struct file *filp, const char __user *buf, 
                size_t count, loff_t *f_pos) 
{
        //ssize_t result = -ENOMEM;
        ssize_t result = 0;
	char ch;
        if (num < 10) {
                /*
                 * Copy data from user space to kernel space.
                 * Returns number of bytes that could not be copied. 
                 * On success, this will be zero.
                 * If some data could not be copied, this function will pad 
                 * the copied data to the requested size using zero bytes.
                 */
                if(copy_from_user(&ch, buf, 1)) {
                        result = -EFAULT; /* Bad address */
                        goto out;
                }
                tampon[num++] = ch;
                printk(KERN_WARNING"scull_write (%s:%u) count = %lu ch = %c\n",
                        __FUNCTION__, __LINE__, count, ch);
                return 1;
        } else {
                printk(KERN_WARNING"scull_write (%s:%u) count = %lu ch = no place\n",
                        __FUNCTION__, __LINE__, count);
                return 0;
        }
out:
        return result;
}



/*
 * TODO: Validate IS_ERR and PTR_ERR in err.h
 *       are same to if(!ptr){res = -ENOMEM; goto err_ptr} 
 */
static int __init scull_init (void) {

        int result = 0;

        printk(KERN_ALERT"scull_init");
        /* ring buffer initialization */
        ring_buf.buffer = (unsigned short*) 
            kmalloc(DEFAULT_BUFSIZE * sizeof(unsigned short), GFP_KERNEL);
        if(IS_ERR(ring_buf.buffer))
                return ERR_PTR(-ENOMEM); /* Out of memory */
        ring_buf.buf_empty = 1;
        ring_buf.buf_full  = 0;
        ring_buf.buf_size  = DEFAULT_BUFSIZE;
        ring_buf.in_idx    = 0;
        ring_buf.out_idx   = 0;
        /* scull initialization */
        scull_dev.read_buf = (unsigned short *) 
                kmalloc(DEFAULT_RWSIZE * sizeof(unsigned short), GFP_KERNEL);
        if(IS_ERR(scull_dev.read_buf))
                return ERR_PTR(-ENOMEM); /* Out of memory */
        scull_dev.write_buf = (unsigned short *) 
                kmalloc(DEFAULT_RWSIZE * sizeof(unsigned short), GFP_KERNEL);
        if(IS_ERR(scull_dev.write_buf))
                return ERR_PTR(-ENOMEM); /* Out of memory */
        scull_dev.num_writer = 0;
        scull_dev.num_reader = 0;
        /*
         * Allocates a range of char device numbers. 
         * The major number will be chosen dynamically and returned 
         * (along with the first minor number) in dev. 
         * Returns zero or a negative error code.
         */
        result = alloc_chrdev_region(&scull_dev.dev, 0, 1, "scull");
        if(result < 0) {
                printk(KERN_ERR "failed to alloc chrdev region\n");
                goto fail_alloc_chrdev_region;
        }
        /*
         * This is used to create a struct class pointer that can then be used 
         * in calls to class_device_create.
         * Note, the pointer created here is to be destroyed when finished
         * by making a call to class_destroy.
         */
        class_dev = class_create(THIS_MODULE, "scull_class");
        if(!class_dev) {
                result = -EEXIST; /* File exists */
                printk(KERN_ERR "failed to create class\n");
                goto fail_create_class;
        }
        /*
         * A struct device will be created in sysfs, registered to the specified
         * class.
         * 
         * A “dev” file will be created, showing the dev_t for the device,
         * if the dev_t is not 0,0.
         * If a pointer to a parent struct device is passed in, the newly 
         * created struct device will be a child of that device in sysfs.
         * The pointer to the struct device will be returned from the call.
         * Any further sysfs files that might be required can be created using 
         * this pointer.
         */
        if(!(device_create(class_dev, NULL, scull_dev.dev, NULL, "scull_Node"))) {
                result = -EINVAL; /* Invalid argument */
                printk(KERN_ERR "failed to create device\n");
                goto fail_create_device;
        }
        /*
         * Initializes cdev, remembering fops, making it ready to add 
         * to the system with cdev_add.
         */
        cdev_init(&scull_dev.cdev, &scull_fops);
        scull_dev.cdev.owner = THIS_MODULE;
        if (cdev_add(&scull_dev.cdev, scull_dev.dev, 1) < 0)
                printk(KERN_WARNING"scull ERROR IN cdev_add\n");
        return 0;

fail_create_device:
        class_destroy(class_dev);
fail_create_class:
        cdev_del(&scull_dev.cdev);
fail_alloc_chrdev_region:
        kfree(ring_buf.buffer);
        kfree(scull_dev.read_buf);
        kfree(scull_dev.write_buf);
        return result;
}

static void __exit scull_exit (void) {
        /* free pointers */
        kfree(ring_buf.buffer);
        kfree(scull_dev.read_buf);
        kfree(scull_dev.write_buf);
        /* remove device and class device */       
        cdev_del(&scull_dev.cdev);
        unregister_chrdev_region(scull_dev.dev, 1);
        device_destroy (class_dev, scull_dev.dev);
        class_destroy(class_dev);

        printk(KERN_ALERT"scull_exit");
}

module_init(scull_init);
module_exit(scull_exit);
