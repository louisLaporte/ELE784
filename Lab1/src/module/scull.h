
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

/************************ Définitions ********************************/
#define DEFAULT_RWSIZE 16
#define DEFAULT_BUFSIZE 256
#define MAX_USER 10
/***************************** Structures ************************************/
struct ring_buffer {
    unsigned int    InIdx   ;
    unsigned int    OutIdx  ;
    unsigned short  BufFull ;
    unsigned short  BufEmpty;
    unsigned int    BufSize ;
    char            *Buffer ;
};
struct scull {
    char            *ReadBuf    ;
    char            *WriteBuf   ;
    unsigned short  numWriter   ;
    unsigned short  maxReader   ;
    dev_t           dev         ;
    struct cdev     cdev        ;
};

int scull_open(struct inode *inode, struct file *filp);
int scull_release(struct inode *inode, struct file *filp);
static ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
static ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);

static int   scull_init (void);
static void  scull_exit (void);
struct scull s;
struct ring_buffer r;
struct class *c;

int scull_var = 0;
module_param(scull_var, int, S_IRUGO);
EXPORT_SYMBOL_GPL(scull_var);

char 	 tampon[10] = {0,0,0,0,0,0,0,0,0,0};
uint16_t num = 0;
