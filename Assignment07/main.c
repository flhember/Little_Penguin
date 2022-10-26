// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/jiffies.h>
#include <linux/string.h>
#include <linux/kthread.h>


MODULE_LICENSE("GPL");
MODULE_AUTHOR("flhember");
MODULE_DESCRIPTION("Debugfs module");

#define SIZE_LOGIN 8
#define PAGESIZE 4096

struct dentry *main_dir;
struct dentry *id_file;
struct dentry *jiffies_file;
struct dentry *foo_file;

struct mutex etx_mutex;
static char foo_data[PAGESIZE];
static size_t foo_data_len;

static ssize_t id_write(struct file *file, const char __user *user_buffer, size_t user_len, loff_t *pos)
{
	char buff[8];
	char *login = "flhember";
	size_t size = strlen(login);

	if (user_len != size) {
		pr_info("Bad size, good size is %zu\n", size);
		return -EINVAL;
	}
	if (!simple_write_to_buffer(buff, size, pos, user_buffer, user_len))
		return -EINVAL;
	if (strncmp(buff, login, size) == 0) {
		pr_info("Good Value\n");
		return user_len;
	}
	pr_info("Bad Value\n");
	return -EINVAL;
}

static ssize_t id_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *pos)
{
	char *login = "flhember\n";

	return simple_read_from_buffer(user_buffer, user_len, pos, login, strlen(login));
}

static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write
};

static ssize_t jiffies_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *pos)
{
	char buffer[50];
	unsigned long jiffies_value = jiffies;

	sprintf(buffer, "%lu", jiffies_value);
	strcat(buffer, "\n");
	return simple_read_from_buffer(user_buffer, user_len, pos, buffer, strlen(buffer));
}

static const struct file_operations jiffies_fops = {
	.owner = THIS_MODULE,
	.read = jiffies_read
};

static ssize_t foo_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *pos)
{
	return simple_read_from_buffer(user_buffer, user_len, pos, foo_data, foo_data_len);
}

static ssize_t foo_write(struct file *file, const char __user *user_buffer, size_t user_len, loff_t *pos)
{
	ssize_t ret = 0;

	mutex_lock(&etx_mutex);
	memset(foo_data, '\0', PAGESIZE);
	if (user_len >= PAGESIZE) {
		pr_info("Bad Value - Size to big\n");
		mutex_unlock(&etx_mutex);
		return -EINVAL;
	}
	foo_data_len = user_len;
	ret = simple_write_to_buffer(foo_data, PAGE_SIZE, pos, user_buffer, user_len);
	mutex_unlock(&etx_mutex);
	return ret;
}

static const struct file_operations foo_fops = {
	.owner = THIS_MODULE,
	.read = foo_read,
	.write = foo_write
};

static int __init ModuleInit(void)
{
	mutex_init(&etx_mutex);
	main_dir = debugfs_create_dir("fortytwo", NULL);
	id_file = debugfs_create_file("id", 0666, main_dir, NULL, &id_fops);
	jiffies_file = debugfs_create_file("jiffies", 0444, main_dir, NULL, &jiffies_fops);
	foo_file = debugfs_create_file("foo", 0644, main_dir, NULL, &foo_fops);

	return 0;
}

static void __exit ModuleExit(void)
{
	pr_info("Cleaning up module.\n");
	debugfs_remove_recursive(main_dir);
}

module_init(ModuleInit);
module_exit(ModuleExit);
