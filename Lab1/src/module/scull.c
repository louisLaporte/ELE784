#include "scull.h"
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define DEBUG 0
struct file_operations scull_fops = {
	.owner	        =	THIS_MODULE,
	.read		=	scull_read,
	.write	        =	scull_write,
	.open		=	scull_open,
	.release	=	scull_release
};

void debug_buf(struct ring_buffer *buf) 
{
    printk(KERN_WARNING"buf->in_idx    %d\n",buf->in_idx   );
    printk(KERN_WARNING"buf->out_idx   %d\n",buf->out_idx  );
    printk(KERN_WARNING"buf->buf_full  %d\n",buf->buf_full );
    printk(KERN_WARNING"buf->buf_empty %d\n",buf->buf_empty);
    printk(KERN_WARNING"buf->buf_size  %d\n",buf->buf_size );
}
int buf_in(struct ring_buffer *buf, unsigned char *data)
{
        if (buf->buf_full)
            return -1;
        buf->buf_empty = 0;
        buf->buffer[buf->in_idx] = *data;
        buf->in_idx = (buf->in_idx + 1) % buf->buf_size;
        if (buf->in_idx == buf->out_idx)
              buf->buf_full = 1;
#if DEBUG
        debug_buf(buf);
#endif
        return 0;
}

int buf_out(struct ring_buffer *buf, unsigned char *data)
{
        if (buf->buf_empty)
                return -1;
        buf->buf_full = 0;
        *data = buf->buffer[buf->out_idx];
        buf->out_idx = (buf->out_idx + 1) % buf->buf_size;
        if (buf->out_idx == buf->in_idx)
                buf->buf_empty = 1;
#if DEBUG
        debug_buf(buf);
#endif
        return 0;
}


int scull_open(struct inode *inode, struct file *filp)
{

        printk(KERN_WARNING"scull_open\n");

        switch ((filp->f_flags & O_ACCMODE)) {
        down_interruptible(&scull_dev.sem_buf);
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
        up(&scull_dev.sem_buf);
        printk(KERN_WARNING"nb: reader = %d | writer = %d ", 
                        scull_dev.num_reader, scull_dev.num_writer);
        return 0;
}

int scull_release(struct inode *inode, struct file *filp)
{
        
        printk(KERN_WARNING"scull_release\n");

        switch (filp->f_flags & O_ACCMODE) {
        down_interruptible(&scull_dev.sem_buf);
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
        up(&scull_dev.sem_buf);
        printk(KERN_WARNING"nb: reader = %d | writer = %d ", 
                        scull_dev.num_reader, scull_dev.num_writer);
        return 0;
}

static ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos) 
{
        ssize_t  retval = 0; 
        int i,  lu, done, max, to_user;
        i = 0;
        lu = 0;
        done = 0;
        max = 0;
        to_user = 0;
        
        if ((filp->f_flags & O_NONBLOCK) == 0) {
                if (down_trylock(&scull_dev.sem_buf))
                        return -EAGAIN;
        }
        else if (down_interruptible(&scull_dev.sem_buf))
                return -ERESTARTSYS;

        while (count > 0) {
            /* max buffer size */
            max = count > DEFAULT_RWSIZE ? DEFAULT_RWSIZE : count;
            printk(KERN_ALERT"%s to_user: %d\n", __FUNCTION__, max);

            for (i = 0; i < max; i++) {
                    /* dequeue buffer */
                    if (buf_out(&ring_buf, &scull_dev.read_buf[i]) == -1)
                            break;
                    to_user++;
                    printk(KERN_ALERT"to_user: %d\n",to_user);
            }
            if (copy_to_user(buf, scull_dev.read_buf, to_user)) {
                    retval = -EFAULT;
                    up (&scull_dev.sem_buf);
            }
            buf += to_user;
            to_user = 0;
            count -= max;
            printk(KERN_ALERT"count %lu\n",count);
        }


        up(&scull_dev.sem_buf);
        return retval;
}

static ssize_t scull_write(struct file *filp, const char __user *buf, 
                size_t count, loff_t *f_pos) 
{
        //printk(KERN_ERR "count %lu | user: %s \n", count, &buf);

        ssize_t retval = 0;
        int i, done, max;
        i = 0;
        done = 0;
        max = 0;
        
        if ((filp->f_flags & O_NONBLOCK) == 0) {
                if (down_trylock(&scull_dev.sem_buf))
                        return -EAGAIN;
        }
        else if (down_interruptible(&scull_dev.sem_buf))
                return -ERESTARTSYS;

        while (count > 0) {
                max = count > DEFAULT_RWSIZE ? DEFAULT_RWSIZE : count;
                printk(KERN_ALERT"%s -- count %lu\n", __FUNCTION__, count);
                /*
                 * Copy data from user space to kernel space.
                 * Returns number of bytes that could not be copied. 
                 * On success, this will be zero.
                 * If some data could not be copied, this function will pad 
                 * the copied data to the requested size using zero bytes.
                 */
                if (copy_from_user(&scull_dev.write_buf[0], buf, max)) {
                        retval = -EFAULT;
                }
                for (i = 0; i < max; i++) {
                        if (buf_in(&ring_buf, &scull_dev.write_buf[i]) == -1)
                                break;
                        printk(KERN_ALERT"%s -- i %d\n", __FUNCTION__, i);
                }
                count -= max;
        }
        up(&scull_dev.sem_buf);

        return retval; 
}
long scull_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int err = 0;
    int retval = 0;
    int count = 0;
    int nSize = 0;
    int i;
    unsigned char  *temp_rb, *temp_rb_del;

    printk(KERN_WARNING"scull_ioctl\n");


    if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) 
            return -ENOTTY;

    if (_IOC_NR(cmd) > SCULL_IOC_MAXNR)
            return -ENOTTY;

    if (_IOC_DIR(cmd) & _IOC_READ)
            err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
            err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));

    if (err)
            return -EFAULT;

    switch (cmd) {
            case SCULL_GETNUMDATA :   
                    retval = __put_user(ring_buf.count, (int __user *)arg);
                    break;

            case SCULL_GETNUMREADER :   
                    retval = __put_user(scull_dev.num_reader, (int __user *)arg);
                    break;

            case SCULL_GETBUFSIZE :   
                    retval = __put_user(ring_buf.buf_size, (int __user *)arg);
                    break;

            case SCULL_SETBUFSIZE :   
                    if (!capable (CAP_SYS_ADMIN))
                            return -EPERM;

                    retval = __get_user(nSize, (int __user *)arg);

                    count = ring_buf.count;

                    if (count > nSize)
                            return -ENOMEM;


                    if (down_trylock(&scull_dev.sem_buf))
                            return -EAGAIN;

                    ring_buf.buf_size = nSize;

                    temp_rb = (unsigned char *) 
                        kmalloc(ring_buf.buf_size * sizeof(unsigned char), GFP_KERNEL);

                    for (i = 0; i < count; i++)
                            buf_out(&ring_buf, &temp_rb[i]);

                    temp_rb_del = ring_buf.buffer;
                    ring_buf.buffer = temp_rb;
                    kfree(temp_rb_del);

                    ring_buf.count = count;

                    if (count == 0)
                    {
                            ring_buf.buf_full = 0;
                            ring_buf.buf_empty = 1;
                            ring_buf.in_idx = 0;

                    } else if (count == nSize) {
                            ring_buf.buf_full = 1;
                            ring_buf.buf_empty = 0;
                            ring_buf.in_idx = nSize;
                    } else {
                            ring_buf.buf_full = 0;
                            ring_buf.buf_empty = 0;
                            ring_buf.in_idx = nSize;
                    }

                    up(&scull_dev.sem_buf);

                    break;
		default : 
                        return -ENOTTY;
	}
	return retval;
}

static int __init scull_init (void)
{

        int retval = 0;

        printk(KERN_ALERT"\nscull_init\n");
        /* ring buffer initialization */
        ring_buf.buffer = (unsigned char*) 
            kmalloc(DEFAULT_BUFSIZE * sizeof(unsigned char), GFP_KERNEL);
        if (!ring_buf.buffer)
                return -ENOMEM; 
        ring_buf.buf_empty = 1;
        ring_buf.buf_full  = 0;
        ring_buf.buf_size  = DEFAULT_BUFSIZE;
        ring_buf.in_idx    = 0;
        ring_buf.out_idx   = 0;
        /* scull initialization */
        scull_dev.read_buf = (unsigned char *) 
                kmalloc(DEFAULT_RWSIZE * sizeof(unsigned char), GFP_KERNEL);
        if (!scull_dev.read_buf)
                return -ENOMEM;
        scull_dev.write_buf = (unsigned char *) 
                kmalloc(DEFAULT_RWSIZE * sizeof(unsigned char), GFP_KERNEL);
        if (!scull_dev.write_buf)
                return -ENOMEM;
        scull_dev.num_writer = 0;
        scull_dev.num_reader = 0;
        sema_init(&scull_dev.sem_buf, 1);
        /*
         * Allocates a range of char device numbers. 
         * The major number will be chosen dynamically and returned 
         * (along with the first minor number) in dev. 
         * Returns zero or a negative error code.
         */
        retval = alloc_chrdev_region(&scull_dev.dev, 0, 1, "scull");
        if (retval < 0) {
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
        if (!class_dev) {
                retval = -EEXIST; /* File exists */
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
                retval = -EINVAL; /* Invalid argument */
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
        return retval;
}

static void __exit scull_exit (void)
{
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
