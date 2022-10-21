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
MODULE_DESCRIPTION("testetst"); // a faire

static struct proc_dir_entry *mymounts_file;

static char *print_mount(const char *name, struct path p)
{
	size_t size;
	char *path;
	char buf[1024];
	char *final = NULL;

	size = strlen(name) + 1;
	path = d_path(&p, buf, 1024);
	size += strlen(path);

	final = kmalloc(size, GFP_USER);
	strcpy(final, name);
	strcat(final, " ");
	strcat(final, path);
	return final;
}

static ssize_t my_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *pos)
{
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct mount *mnt;
	struct path p;
	char *final;
	size_t size;

	list_for_each_entry(mnt, &ns->list, mnt_list) {
		p.mnt = &mnt->mnt;
		p.dentry = mnt->mnt.mnt_root;
		final = print_mount(mnt->mnt_devname, p);
		size = strlen(final);
		pr_info("!%s!\n", final);
		simple_read_from_buffer(user_buffer, user_len, pos, "salut", 5);
	}
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
