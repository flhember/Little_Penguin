#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("flhember");
MODULE_DESCRIPTION("Module load when keyboard detect");

static int __init ModuleInit(void)
{
	pr_info("My keyboard module load!\n");
	return 0;
}

static void __exit ModuleExit(void)
{
	pr_info("My keyboard module clean!\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
