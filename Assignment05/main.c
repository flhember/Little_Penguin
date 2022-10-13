// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("flhember");
MODULE_DESCRIPTION("Misc driver");

#define SIZE_LOGIN 8

static ssize_t my_write(struct file *file, const char __user *user_buffer, size_t user_len, loff_t *pos)
{
	char *login = "flhember";
	char buff[SIZE_LOGIN];

	if (user_len - 1 != SIZE_LOGIN) {
		pr_info("Bad size\n");
		return -EINVAL;
	}
	if (copy_from_user(buff, user_buffer, 8))
		return -EINVAL;
	if (strncmp(buff, login, user_len - 1) == 0) {
		pr_info("Good Value\n");
		return user_len;
	}
	pr_info("Bad Value\n");
	return -EINVAL;
}

static ssize_t my_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *pos)
{
	char *login = "flhember\n";
	size_t len = strlen(login);

	if (user_len < len)
		return -EINVAL;
	if (*pos != 0)
		return 0;
	if (copy_to_user(user_buffer, login, len))
		return -EINVAL;
	*pos = len;
	return len;
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
