#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("flhember");
MODULE_DESCRIPTION("Misc driver");

#define MAX_SIZE 256
static char data[MAX_SIZE];
static size_t data_len;

static ssize_t my_write(struct file *file, const char __user *user_buffer, size_t user_len, loff_t *pos) {
	pr_info("Test misc write, %zu\n", user_len);

	return EINVAL;
}

static ssize_t my_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *pos) {
	pr_info("Test misc read\n");

	return 0;
}

static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write
};

static struct miscdevice my_device = {
	.name = "fortytwo",
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &fops
};

static int __init ModuleInit(void)
{
	int status;

	pr_info("Register misc device\n");
	status = misc_register(&my_device);
	if (status) {
		pr_info("Error register misc device\n");
		return -1;
	}
	return 0;
}

static void __exit ModuleExit(void)
{
	pr_info("Cleaning up module.\n");
	misc_deregister(&my_device);
}

module_init(ModuleInit);
module_exit(ModuleExit);
