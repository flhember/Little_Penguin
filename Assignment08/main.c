// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Louis Solofrizzo <louis@ne02ptzero.me>");
MODULE_DESCRIPTION("Useless module");

static ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs);
static ssize_t myfd_write(struct file *fp, const char __user *user, size_t size, loff_t *offs);

static const struct file_operations myfd_fops = {
	.owner = THIS_MODULE,
	.read = &myfd_read,
	.write = &myfd_write
};

static struct miscdevice myfd_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "reverse",
	.fops = &myfd_fops
};

char str[PAGE_SIZE];

static int __init myfd_init(void)
{
	int retval;

	retval = misc_register(&myfd_device);
	return retval;
}

static void __exit myfd_cleanup(void)
{
	misc_deregister(&myfd_device);
}

ssize_t myfd_read(struct file *fp, char __user *user, size_t size, loff_t *offs)
{
	size_t t;
	size_t s;
	char *tmp;

	/** Malloc like a boss **/
	tmp = kmalloc(PAGE_SIZE, GFP_KERNEL);
	memset(tmp, '\0', PAGE_SIZE);
	for (s = strlen(str) - 1, t = 0; t < strlen(str); s--, t++)
		tmp[t] = str[s];
	tmp[t] = 0x0;
	return simple_read_from_buffer(user, size, offs, tmp, t);
}

ssize_t myfd_write(struct file *fp, const char __user *user, size_t size, loff_t *offs)
{
	ssize_t res;

	memset(str, '\0', PAGE_SIZE);
	res = simple_write_to_buffer(str, PAGE_SIZE, offs, user, size);
	return res;
}

module_init(myfd_init);
module_exit(myfd_cleanup);
