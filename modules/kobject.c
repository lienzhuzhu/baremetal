#include <linux/init.h>
#include <linux/module.h>

#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/sched.h>

static const char *input;

//TODO
static ssize_t driver_read() {
    return simple_read_from_buffer(user, size, off, "Hello!\n", 7);
}

//TODO
ssize_t driver_write() {
    return 0;
}

static struct kobj_attribute driver_attribute = {
    __ATTR(input, 0664, driver_read, driver_write)
};

static struct attribute *attr[] = {
    &driver_attribute.attr,
    NULL,
};

static struct attribute_group attr_group = {
    .attrs = attr,
};

static struct kobject *driver_kobj;

static int driver_init(void) {
    int ret;

    printk("loading driver module\n");

    driver_kobj = kobject_create_and_add("fam_reader", kernel_kobj);
	if (!driver_kobj) {
		return -ENOMEM;
    }

	/* Create the files associated with this kobject */
	ret = sysfs_create_group(driver_kobj, &attr_group);
	if (ret != 0) {
		kobject_put(driver_kobj);
    }

	return ret;
}

static void driver_exit(void) {
    printk("unloading driver module\n");
    kobject_put(driver_kobj);
    return;
}

moudle_init(driver_init);
module_exit(driver_exit);

MODULE_LICENSE("GPL");
MODULE_LICENSE("Lien Zhu");