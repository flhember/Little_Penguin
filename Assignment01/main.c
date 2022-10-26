// SPDX-License-Identifier: GPL-2.0-only

#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("flhember");
MODULE_DESCRIPTION("Hello world module");

static int __init ModuleInit(void)
{
	pr_info("Hello world !\n");
	return 0;
}

static void __exit ModuleExit(void)
{
	pr_info("Cleaning up module.\n");
}

module_init(ModuleInit);
module_exit(ModuleExit);
