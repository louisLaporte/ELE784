#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/device.h>      /* *_create */
#include <linux/cdev.h>
#include <asm/uaccess.h>       /* copy_*_user     */
#include <asm/atomic.h>
#include <linux/errno.h>
#include <linux/err.h>         /* IS_PTR PTR_ERR */
#include <linux/slab.h>        /* kmalloc        */
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#include <linux/capability.h>



MODULE_LICENSE("Dual BSD/GPL");

#define DEFAULT_RWSIZE 16
#define DEFAULT_BUFSIZE 256
#define MAX_WRITER 1
/*
 * ring_buffer
 */
struct ring_buffer {
    unsigned int    in_idx   ;
    unsigned int    out_idx  ;
    unsigned short  buf_full ;
    unsigned short  buf_empty;
    unsigned int    buf_size ;
    unsigned char  *buffer  ;
};
struct scull {
    unsigned char   *read_buf ;
    unsigned char   *write_buf;
    struct semaphore sem_buf   ;
    unsigned short   num_writer;
    unsigned short   num_reader;
    dev_t            dev       ;
    struct cdev      cdev      ;
};

struct scull scull_dev;
struct ring_buffer ring_buf;
struct class *class_dev;

int scull_open(struct inode *inode, struct file *filp);
int scull_release(struct inode *inode, struct file *filp);
static ssize_t scull_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos);

static ssize_t scull_write(struct file *filp, const char __user *buf,
                size_t count, loff_t *f_pos);

static long scull_ioctl (struct file *filp, unsigned int cmd, 
                unsigned long arg);

static int   scull_init (void);
static void  scull_exit (void);


int scull_var = 0;
module_param(scull_var, int, S_IRUGO);
EXPORT_SYMBOL_GPL(scull_var);
