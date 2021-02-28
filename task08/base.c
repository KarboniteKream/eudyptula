// SPDX-License-Identifier: GPL-2.0

#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>

MODULE_AUTHOR("Klemen Košir <klemen.kosir@kream.io>");
MODULE_DESCRIPTION("A simple Linux module");
MODULE_LICENSE("GPL");

static struct dentry *root;

static const char id[] = "2dfea3d4488b";
#define ID_LENGTH ARRAY_SIZE(id)

static ssize_t id_read(struct file *file, char __user *buf,
		       size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(buf, count, ppos, id, ID_LENGTH);
}

static ssize_t id_write(struct file *file, const char __user *buf,
			size_t count, loff_t *ppos)
{
	char kbuf[ID_LENGTH];
	size_t limit = ID_LENGTH - 1;
	ssize_t ret;

	ret = simple_write_to_buffer(kbuf, limit, ppos, buf, count);
	kbuf[limit] = 0;

	if (ret < 0)
		return ret;
	if (ret != limit || strcmp(kbuf, id))
		return -EINVAL;

	return ret;
}

static const struct file_operations id_fops = {
	.owner = THIS_MODULE,
	.read = id_read,
	.write = id_write,
};

static int __init eudyptula_init(void)
{
	root = debugfs_create_dir("eudyptula", NULL);

	debugfs_create_file("id", 0666, root, NULL, &id_fops);

	return 0;
}

static void __exit eudyptula_exit(void)
{
	debugfs_remove_recursive(root);
}

module_init(eudyptula_init);
module_exit(eudyptula_exit);
