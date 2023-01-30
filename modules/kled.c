#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>


#define MAX_SIZE 1024
#define BCM2837_GPIO_ADDRESS 0x3f200000

static int led_state;

static ssize_t led_state_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
	return sprintf(buf, "%d\n", led_state);
}

static ssize_t led_state_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
	int var, ret;

	ret = kstrtoint(buf, 10, &var);
	printk(KERN_DEBUG "led_state update: %d -> %d\n", led_state, var);

	ret = kstrtoint(buf, 10, &led_state);

	if (ret < 0)
		return ret;

	return count;
}

static struct kobj_attribute led_state_attribute = __ATTR(led_state, 0664, led_state_show, led_state_store);

static struct kobject *led_dir;

static int __init led_driver_init(void) {
	int retval;

	led_dir = kobject_create_and_add("led_dir", kernel_kobj);
	if (!led_dir) {
		return -ENOMEM;
	}

	retval = sysfs_create_file(led_dir, &led_state_attribute.attr);
	if (retval) {
		kobject_put(led_dir);
	}

	return retval;
}

static void __exit led_driver_exit(void) {
	kobject_put(led_dir);
}

module_init(led_driver_init);
module_exit(led_driver_exit);
MODULE_LICENSE("GPL v2");