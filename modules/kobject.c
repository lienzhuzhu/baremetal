#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>

static int foo;


static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
			char *buf)
{
	return sprintf(buf, "%d\n", foo);
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	int var, ret;

	ret = kstrtoint(buf, 10, &var);
	printk(KERN_DEBUG "foo update: %d -> %d\n", foo, var);

	ret = kstrtoint(buf, 10, &foo);

	if (ret < 0)
		return ret;

	return count;
}

static struct kobj_attribute foo_attribute = __ATTR(foo, 0664, foo_show, foo_store);

static struct kobject *led_dir;

static int __init example_init(void)
{
	int retval;

	led_dir = kobject_create_and_add("kobject_example", kernel_kobj);
	if (!led_dir) {
		return -ENOMEM;
	}

	retval = sysfs_create_file(led_dir, &foo_attribute.attr);
	if (retval) {
		kobject_put(led_dir);
	}

	return retval;
}

static void __exit example_exit(void)
{
	kobject_put(led_dir);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL v2");