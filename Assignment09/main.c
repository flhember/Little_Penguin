#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>

// test 
#include <linux/dcache.h>
#include <linux/path.h>
#include <linux/mount.h>
//
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/namei.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("flhember");
MODULE_DESCRIPTION("testetst"); // a faire

static struct proc_dir_entry *mymounts_file;

static ssize_t my_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *pos)
{
	pr_info("myread ft !\n");

	struct dentry *curdentry;

	//printk("%s just for test\n", curdentry->d_name);
	/*list_for_each_entry(curdentry, &current->d_subdirs, d_child)
	{
		if ( curdentry->d_flags & DCACHE_MOUNTED)
		printk("%s is mounted", curdentry->d_name.name);
	}*/
	return 0;
}

static struct proc_ops my_fops = {
	.proc_read = my_read
};

static int __init ModuleInit(void)
{
	mymounts_file = proc_create("mymounts", 0660, NULL, &my_fops);
	pr_info("mymounts file create !\n");
	return 0;
}

static void __exit ModuleExit(void)
{
	proc_remove(mymounts_file);
	pr_info("Cleaning up module. rm mymounts file\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
