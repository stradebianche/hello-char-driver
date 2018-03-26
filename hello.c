#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <asm/errno.h>

volatile static int is_open = 0;
volatile static int hello_major = 0;
volatile static int hello_minor = 0;
volatile static int hello_nr_devs = 1;

/* Definition of driver device structure */
struct hello_dev {
	int dev_id;
	struct cdev cdev;	/* Char device structure */
};


/* Global device structure */
struct hello_dev hello_dev;


ssize_t hello_read (struct file *fp, char *buf, size_t nbytes, loff_t *offset) {
	printk(KERN_INFO "Op not supported.\n");
	return -EINVAL;
}

ssize_t hello_write (struct file *fp, const char *buf, size_t nbytes, loff_t *offset) {
	printk(KERN_INFO "Op not supported.\n");
	return -EINVAL;
}

int hello_open (struct inode *inp, struct file *fp) {

	if (is_open == 1) {
		printk(KERN_INFO "HELLO: Device already open.\n");
		return -EBUSY;
	} else {
		is_open = 1;
	}

	try_module_get(THIS_MODULE);

	return 0;
}

int hello_release (struct inode *inp, struct file *fp) {

	if (is_open == 0) {
		printk(KERN_INFO "HELLO: Device not opened.\n");
		return -ENODEV;
	} else {
		is_open = 0;
	}

	module_put(THIS_MODULE);

	return 0;
}


struct file_operations hello_fops = {
	.owner = THIS_MODULE,
	.read = hello_read,
	.write = hello_write,
	.open = hello_open,
	.release = hello_release,
};


static int __init hello_init(void) {

	int result;
	dev_t dev;

	printk(KERN_INFO "HELLO: Hello, module!\n");

	result = alloc_chrdev_region(	&dev,
					hello_minor,
					hello_nr_devs,
					"hello"	);

	hello_major = MAJOR(dev);

	if (result < 0) {
		printk(KERN_WARNING "HELLO: can't get major %d\n", hello_major);
		return result;
	}

	cdev_init(&hello_dev.cdev, &hello_fops);

	hello_dev.cdev.owner = THIS_MODULE;
	hello_dev.cdev.ops = &hello_fops;

	result = cdev_add (&hello_dev.cdev, dev, 1);

	if (result)
		printk(KERN_NOTICE "HELLO: Error %d adding hello", result);

	/* All above code can be replaced with this line */
	//hello_major = register_chrdev(0, "hello", &hello_fops);

	printk(KERN_INFO "HELLO: Received major: %d\n", hello_major);

	return 0;
}

static void __exit hello_exit(void) {

	dev_t devno = MKDEV(hello_major, hello_minor);

	cdev_del(&hello_dev.cdev);

	unregister_chrdev_region(devno, hello_nr_devs);

	/* The 'old way' not preffered */
	//unregister_chrdev(hello_major, "hello");
	printk(KERN_INFO "HELLO: Goodbye, cruel world.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");

