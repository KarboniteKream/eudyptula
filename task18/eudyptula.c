// SPDX-License-Identifier: GPL-2.0

#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/wait.h>

MODULE_AUTHOR("Klemen Košir <klemen.kosir@kream.io>");
MODULE_DESCRIPTION("A simple Linux module");
MODULE_LICENSE("GPL");

#define NAME_LENGTH 20

struct identity {
	struct list_head list;
	char name[NAME_LENGTH];
	int id;
	bool busy;
};

static LIST_HEAD(identity_list);
static DEFINE_MUTEX(list_lock);

static DECLARE_WAIT_QUEUE_HEAD(wee_wait);
static struct task_struct *thread;

static int identity_create(char *name, int id)
{
	struct identity *entry;

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	INIT_LIST_HEAD(&entry->list);
	strcpy(entry->name, name);
	entry->id = id;

	mutex_lock(&list_lock);
	list_add_tail(&entry->list, &identity_list);
	mutex_unlock(&list_lock);

	return 0;
}

static struct identity *identity_get(void)
{
	struct identity *entry;

	mutex_lock(&list_lock);
	entry = list_first_entry_or_null(&identity_list, struct identity, list);

	if (entry)
		list_del(&entry->list);

	mutex_unlock(&list_lock);
	return entry;
}

static ssize_t eudyptula_write(struct file *file, const char __user *buf,
			       size_t count, loff_t *ppos)
{
	ssize_t ret;
	char name[NAME_LENGTH] = {0};
	static int id = 1;

	ret = simple_write_to_buffer(name, sizeof(name), ppos, buf, count);
	name[NAME_LENGTH - 1] = 0;
	if (ret < 0)
		return ret;

	ret = identity_create(name, id);
	if (ret)
		return ret;

	id++;
	wake_up_interruptible(&wee_wait);

	return count;
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

static int thread_main(void *data)
{
	struct identity *entry;

	while (!kthread_should_stop()) {
		if (wait_event_interruptible(wee_wait, kthread_should_stop() ||
					     !list_empty(&identity_list)))
			return -ERESTARTSYS;

		if (kthread_should_stop())
			break;

		entry = identity_get();
		if (!entry)
			continue;

		msleep_interruptible(5000);
		pr_debug("id %d = %s\n", entry->id, entry->name);
		kfree(entry);
	}

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
	struct identity *entry;

	misc_deregister(&eudyptula_device);
	kthread_stop(thread);

	while ((entry = identity_get()))
		kfree(entry);
}

module_init(eudyptula_init);
module_exit(eudyptula_exit);
