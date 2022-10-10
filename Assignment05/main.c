#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("flhember");
MODULE_DESCRIPTION("Misc driver");

static struct miscdevice my_device = {
	.name = "fortytwo",
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &fops
};

static int __init ModuleInit(void)
{
	int status;

	pr_info("Register misc device\n");
	status = /* ft register with &my_device*/
	if (status) {
		pr_info("Error register misc device\n");
		return -1;
	}
	return 0;
}

static void __exit ModuleExit(void)
{
	pr_info("Cleaning up module.\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
