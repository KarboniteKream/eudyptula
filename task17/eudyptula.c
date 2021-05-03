// SPDX-License-Identifier: GPL-2.0

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/wait.h>

MODULE_AUTHOR("Klemen Košir <klemen.kosir@kream.io>");
MODULE_DESCRIPTION("A simple Linux module");
MODULE_LICENSE("GPL");

static const char ID[] = "2dfea3d4488b";
#define ID_LENGTH ARRAY_SIZE(ID)

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
	.write = eudyptula_write,
};

static struct miscdevice eudyptula_device = {
	.name = "eudyptula",
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &eudyptula_fops,
	.mode = 0222,
};

static DECLARE_WAIT_QUEUE_HEAD(wee_wait);
static struct task_struct *thread;

static int thread_main(void *data)
{
	wait_event(wee_wait, kthread_should_stop());
	return 0;
}

static int __init eudyptula_init(void)
{
	thread = kthread_run(thread_main, NULL, "eudyptula");

	if (IS_ERR(thread))
		return PTR_ERR(thread);

	return misc_register(&eudyptula_device);
}

static void __exit eudyptula_exit(void)
{
	misc_deregister(&eudyptula_device);
	kthread_stop(thread);
}

module_init(eudyptula_init);
module_exit(eudyptula_exit);
