#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>


MODULE_LICENSE("Dual BSD/GPL");

int scull_var = 0;
module_param(scull_var, int, S_IRUGO);

EXPORT_SYMBOL_GPL(scull_var);

dev_t devno;
struct class *scull_class;
struct cdev  scull_cdev;

char 	 tampon[10] = {0,0,0,0,0,0,0,0,0,0};
uint16_t num = 0;

int scull_open(struct inode *inode, struct file *filp) {
	printk(KERN_WARNING"HelloDev_open (%s:%u)\n", __FUNCTION__, __LINE__);
	return 0;
}

int scull_release(struct inode *inode, struct file *filp) {
	printk(KERN_WARNING"HelloDev_release (%s:%u)\n", __FUNCTION__, __LINE__);
	return 0;
}

static ssize_t scull_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
	char ch;

	if (num > 0) {
		ch = tampon[--num];
/***
 * Copy data from kernel space to user space.
 * Returns number of bytes that could not be copied. On success, this will be zero.
 */
		copy_to_user(buf, &ch, 1);
		printk(KERN_WARNING"scull_read (%s:%u) count = %lu ch = %c\n", __FUNCTION__, __LINE__, count, ch);
		return 1;
	} else {
		printk(KERN_WARNING"scull_read (%s:%u) count = %lu ch = no char\n", __FUNCTION__, __LINE__, count);
		return 0;
	}
}

static ssize_t scull_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos) {
	char ch;

	if (num < 10) {
/***
 * Copy data from user space to kernel space.
 * Returns number of bytes that could not be copied. On success, this will be zero.
 * If some data could not be copied, this function will pad the copied data to the requested size using zero bytes.
 */
		copy_from_user(&ch, buf, 1);
		tampon[num++] = ch;
		printk(KERN_WARNING"scull_write (%s:%u) count = %lu ch = %c\n", __FUNCTION__, __LINE__, count, ch);
		return 1;
	} else {
		printk(KERN_WARNING"scull_write (%s:%u) count = %lu ch = no place\n", __FUNCTION__, __LINE__, count);
		return 0;
	}
}

struct file_operations scull_fops = {
	.owner	        =	THIS_MODULE,
	.read		=	scull_read,
	.write	        =	scull_write,
	.open		=	scull_open,
	.release	=	scull_release
};


static int __init hellodev_init (void) {
	int result;

	printk(KERN_ALERT"scull_init (%s:%u) => Hello, World !!!\n", __FUNCTION__, __LINE__);
/***
 * Allocates a range of char device numbers. The major number will be chosen dynamically,
 * and returned (along with the first minor number) in dev. 
 * Returns zero or a negative error code.
 */
	result = alloc_chrdev_region(&devno, 0, 1, "scull");
	if (result < 0)
		printk(KERN_WARNING"scull_init ERROR IN alloc_chrdev_region (%s:%s:%u)\n", __FILE__, __FUNCTION__, __LINE__);
	else
		printk(KERN_WARNING"scull_init : MAJOR = %u MINOR = %u (scull_var = %u)\n", MAJOR(devno), MINOR(devno), scull_var);
/***
 * This is used to create a struct class pointer that can then be used in calls to class_device_create.
 * Note, the pointer created here is to be destroyed when finished by making a call to class_destroy.
 */
	scull_class = class_create(THIS_MODULE, "scullClass");
/***
 * A struct device will be created in sysfs, registered to the specified class.
 *
 * A “dev” file will be created, showing the dev_t for the device, if the dev_t is not 0,0.
 * If a pointer to a parent struct device is passed in,
 * the newly created struct device will be a child of that device in sysfs.
 * The pointer to the struct device will be returned from the call.
 * Any further sysfs files that might be required can be created using this pointer.
 **/
	device_create(scull_class, NULL, devno, NULL, "scull_Node");
/***
 * Initializes cdev, remembering fops, making it ready to add to the system with cdev_add.
 */
        cdev_init(&scull_cdev, &scull_fops);
	scull_cdev.owner = THIS_MODULE;
	if (cdev_add(&scull_cdev, devno, 1) < 0)
		printk(KERN_WARNING"scull ERROR IN cdev_add (%s:%s:%u)\n", __FILE__, __FUNCTION__, __LINE__);

	return 0;
}

static void __exit hellodev_exit (void) {
	cdev_del(&scull_cdev);
	unregister_chrdev_region(devno, 1);
	device_destroy (scull_class, devno);
	class_destroy(scull_class);

	printk(KERN_ALERT"scull_exit (%s:%u) => Goodbye, cruel world\n", __FUNCTION__, __LINE__);
}

module_init(hellodev_init);
module_exit(hellodev_exit);
