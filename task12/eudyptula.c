// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>

MODULE_AUTHOR("Klemen Košir <klemen.kosir@kream.io>");
MODULE_DESCRIPTION("A simple Linux module");
MODULE_LICENSE("GPL");

struct identity {
	struct list_head list;
	char name[20];
	int id;
	bool busy;
};

static LIST_HEAD(identity_list);

int identity_create(char *name, int id)
{
	struct identity *entry;

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	INIT_LIST_HEAD(&entry->list);
	strcpy(entry->name, name);
	entry->id = id;
	list_add_tail(&entry->list, &identity_list);

	return 0;
}

struct identity *identity_find(int id)
{
	struct identity *entry;

	list_for_each_entry(entry, &identity_list, list) {
		if (entry->id == id)
			return entry;
	}

	return NULL;
}

void identity_destroy(int id)
{
	struct identity *entry;

	list_for_each_entry(entry, &identity_list, list) {
		if (entry->id == id) {
			list_del(&entry->list);
			kfree(entry);
			return;
		}
	}
}

static int __init eudyptula_init(void)
{
	struct identity *temp;
	int ret = 0;

	ret = identity_create("Alice", 1);
	if (ret)
		goto out;

	ret = identity_create("Bob", 2);
	if (ret)
		goto out;

	ret = identity_create("Dave", 3);
	if (ret)
		goto out;

	ret = identity_create("Gena", 10);
	if (ret)
		goto out;

	temp = identity_find(3);
	pr_debug("id 3 = %s\n", temp->name);

	temp = identity_find(42);
	if (!temp)
		pr_debug("id 42 not found\n");

out:
	identity_destroy(2);
	identity_destroy(1);
	identity_destroy(10);
	identity_destroy(42);
	identity_destroy(3);

	return ret;
}

static void __exit eudyptula_exit(void)
{
}

module_init(eudyptula_init);
module_exit(eudyptula_exit);
