// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/string.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("flhember");
MODULE_DESCRIPTION("Debugfs module");

#define SIZE_LOGIN 8
#define PAGESIZE 4096

struct dentry *main_dir;
struct dentry *id_file;
struct dentry *jiffies_file;
struct dentry *foo_file;

static char foo_data[PAGESIZE];
static size_t foo_data_len;

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

static ssize_t foo_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *pos)
{
	size_t len;

        if (user_len < foo_data_len)
                len = user_len;
        else
                len = foo_data_len;
        if (copy_to_user(user_buffer, foo_data, 10))
                return -EINVAL;
        return len;
}

static ssize_t foo_write(struct file *file, const char __user *user_buffer, size_t user_len, loff_t *pos)
{
	if (user_len > foo_data_len)
		foo_data_len = PAGESIZE;
	else
		foo_data_len = user_len;
	if (copy_from_user(foo_data, user_buffer, foo_data_len))
                return -EINVAL;
	return foo_data_len;
}

static const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write
};

static int __init ModuleInit(void)
{
	pr_info("size page = %d\n", PAGESIZE);
	memset(foo_data, '\0', PAGESIZE);
	main_dir = debugfs_create_dir("fortytwo", NULL);
	id_file = debugfs_create_file("id", 0666, main_dir, NULL, &id_fops);
	jiffies_file = debugfs_create_file("jiffies", 0444, main_dir, NULL, &jiffies_fops);
	foo_file = debugfs_create_file("foo", 0777, main_dir, NULL, &foo_fops);

	return 0;
}

static void __exit ModuleExit(void)
{
	pr_info("Cleaning up module.\n");
	debugfs_remove_recursive(main_dir);
}

module_init(ModuleInit);
module_exit(ModuleExit);
