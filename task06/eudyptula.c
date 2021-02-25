// SPDX-License-Identifier: GPL-2.0

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>

MODULE_AUTHOR("Klemen Košir <klemen.kosir@kream.io>");
MODULE_DESCRIPTION("A simple Linux module");
MODULE_LICENSE("GPL");

static const char ID[] = "2dfea3d4488b";
#define ID_LENGTH ARRAY_SIZE(ID)

static ssize_t eudyptula_read(struct file *file, char __user *buf,
			      size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(buf, count, ppos, ID, ID_LENGTH);
}

static ssize_t eudyptula_write(struct file *file, const char __user *buf,
			       size_t count, loff_t *ppos)
{
	char kbuf[ID_LENGTH];
	size_t limit = ID_LENGTH - 1;
	ssize_t ret;

	ret = simple_write_to_buffer(kbuf, limit, ppos, buf, count);
	kbuf[limit] = 0;

	if (ret < 0)
		return ret;
	if (ret != limit || strcmp(kbuf, ID))
		return -EINVAL;

	return ret;
}

static const struct file_operations eudyptula_fops = {
	.owner = THIS_MODULE,
	.read = eudyptula_read,
	.write = eudyptula_write,
};

static struct miscdevice eudyptula_device = {
	.name = "eudyptula",
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &eudyptula_fops,
};

static int __init eudyptula_init(void)
{
	return misc_register(&eudyptula_device);
}

static void __exit eudyptula_exit(void)
{
	misc_deregister(&eudyptula_device);
}

module_init(eudyptula_init);
module_exit(eudyptula_exit);
