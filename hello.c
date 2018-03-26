#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/errno.h>

volatile static int is_open = 0;

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

	int devnum = 0;

	printk(KERN_INFO "Hello, module!\n");

	devnum = register_chrdev(0, "hello", &hello_fops);
	printk(KERN_INFO "HELLO: Received major: %d\n", devnum);

	return 0;
}

static void __exit hello_exit(void) {

	printk(KERN_INFO "Goodbye, cruel world.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
