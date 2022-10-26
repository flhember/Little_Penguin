// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/nsproxy.h>
#include <linux/fs.h>
#include <../fs/mount.h>
#include <linux/mount.h>
#include <linux/seq_file.h>
#include <linux/poll.h>
#include <linux/ns_common.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("flhember");
MODULE_DESCRIPTION("Module to show mount point");

#define PAGESIZE 4096

static struct proc_dir_entry *mymounts_file;

static void print_mount(const char *name, struct path p, char *final)
{
	char *path;
	char buf[1024];

	path = d_path(&p, buf, 1024);
	strcat(final, name);
	strcat(final, " ");
	strcat(final, path);
	strcat(final, "\n");
}

static ssize_t my_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *pos)
{
	int ret;
	size_t size;
	struct path p;
	char *final_buf;
	struct mount *mnt;
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;

	if (*pos != 0)
		return 0;
	final_buf = kmalloc(PAGE_SIZE, GFP_KERNEL);
	memset(final_buf, '\0', PAGE_SIZE);
	list_for_each_entry(mnt, &ns->list, mnt_list) {
		p.mnt = &mnt->mnt;
		p.dentry = mnt->mnt.mnt_root;
		print_mount(mnt->mnt_devname, p, final_buf);
	}
	strcat(final_buf, "\0");
	size = strlen(final_buf);
	ret = simple_read_from_buffer(user_buffer, user_len, pos, final_buf, size);
	kfree(final_buf);
	return ret;
}

static const struct proc_ops my_fops = {
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
