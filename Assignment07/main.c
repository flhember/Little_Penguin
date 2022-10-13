// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("flhember");
MODULE_DESCRIPTION("Debugfs module");

#define SIZE_LOGIN 8

struct dentry *main_dir;
struct dentry *id_file;
struct dentry *jiffies_file;

static ssize_t id_write(struct file *file, const char __user *user_buffer, size_t user_len, loff_t *pos)
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

static ssize_t id_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *pos)
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

static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write
};

static ssize_t jiffies_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *pos)
{
	char buffer[50];
	size_t len = 0;
	unsigned long jiffies_value = jiffies;

	sprintf(buffer, "%lu", jiffies_value);
	strcat(buffer, "\n");
	len = strlen(buffer);
	if (user_len < len)
		return -EINVAL;
	if (*pos != 0)
		return 0;
	if (copy_to_user(user_buffer, buffer, len))
		return -EINVAL;
	*pos = len;
	return len;
}

static const struct file_operations jiffies_fops = {
	.owner = THIS_MODULE,
	.read = jiffies_read
};

static int __init ModuleInit(void)
{
	main_dir = debugfs_create_dir("fortytwo", NULL);
	id_file = debugfs_create_file("id", 0666, main_dir, NULL, &id_fops);
	jiffies_file = debugfs_create_file("jiffies", 0444, main_dir, NULL, &jiffies_fops);

	return 0;
}

static void __exit ModuleExit(void)
{
	pr_info("Cleaning up module.\n");
	debugfs_remove_recursive(main_dir);
}

module_init(ModuleInit);
module_exit(ModuleExit);
