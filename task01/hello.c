// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>

MODULE_AUTHOR("Klemen Košir <klemen.kosir@kream.io>");
MODULE_DESCRIPTION("A simple Linux module");
MODULE_LICENSE("GPL");

static int __init hello_init(void)
{
	pr_debug("Hello World!\n");
	return 0;
}

static void __exit hello_exit(void)
{
}

module_init(hello_init);
module_exit(hello_exit);
