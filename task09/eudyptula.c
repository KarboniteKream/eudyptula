// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/mutex.h>

MODULE_AUTHOR("Klemen Košir <klemen.kosir@kream.io>");
MODULE_DESCRIPTION("A simple Linux module");
MODULE_LICENSE("GPL");

static struct kobject *root;

static const char id[] = "2dfea3d4488b";
#define ID_LENGTH ARRAY_SIZE(id)

#define DATA_SIZE (PAGE_SIZE + 1)
static char data[DATA_SIZE];
static size_t data_count;
static DEFINE_MUTEX(data_lock);

static ssize_t id_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sysfs_emit(buf, "%s\n", id);
}

static ssize_t id_store(struct kobject *kobj, struct kobj_attribute *attr,
			const char *buf, size_t count)
{
	if (count != ID_LENGTH - 1 || strcmp(buf, id))
		return -EINVAL;

	return count;
}

static ssize_t jiffies_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sysfs_emit(buf, "%lu\n", jiffies);
}

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	mutex_lock(&data_lock);
	memcpy(buf, data, data_count + 1);
	mutex_unlock(&data_lock);

	return data_count;
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	if (count >= DATA_SIZE)
		return -EINVAL;

	mutex_lock(&data_lock);

	memcpy(data, buf, count);
	data[count] = 0;
	data_count = count;

	mutex_unlock(&data_lock);

	return count;
}

static struct kobj_attribute id_attr = __ATTR_RW(id);
static struct kobj_attribute jiffies_attr = __ATTR_RO(jiffies);
static struct kobj_attribute foo_attr = __ATTR_RW(foo);

static struct attribute *attrs[] = {
	&id_attr.attr,
	&jiffies_attr.attr,
	&foo_attr.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};

static int __init eudyptula_init(void)
{
	int ret;

	root = kobject_create_and_add("eudyptula", kernel_kobj);
	if (!root)
		return -ENOMEM;

	ret = sysfs_create_group(root, &attr_group);
	if (ret)
		kobject_put(root);

	return ret;
}

static void __exit eudyptula_exit(void)
{
	kobject_put(root);
}

module_init(eudyptula_init);
module_exit(eudyptula_exit);
