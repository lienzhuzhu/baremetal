#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/proc_fs.h>
#include <linux/slab.h>

#define MAX_SIZE 1024

static struct proc_dir_entry *my_proc = NULL;
static char data_buffer[MAX_SIZE];

ssize_t driver_read(struct file *f, char __user *user, size_t size, loff_t *off) {
	if (copy_to_user(user, "Hello!\n", 7) != 0) {
		printk("copy_to_user() failed\n");
		return -1;
	}

	return 7;
}

ssize_t driver_write(struct file *f, const char __user *user, size_t size, loff_t *off) {

	memset(data_buffer, 0x0, sizeof(data_buffer));

	if (size > MAX_SIZE) {
		size = MAX_SIZE;
	}

	if (copy_from_user(data_buffer, user, size) != 0) {
		printk("copy_from_user() failed\n");
		return -1;
	}

	printk("Data buffer: %s\n", data_buffer);

	return size;
}

static const struct proc_ops fops = {
	.proc_read = driver_read,
	.proc_write = driver_write,
};


int driver_init(void) {
	printk("loaded driver module\n");

	// create entry in procfs
	my_proc = proc_create("lz-gpio", 0666, NULL, &fops);
	if (my_proc == NULL) {
		printk("procfs entry failed to create\n");
		return -1;
	}

	return 0;
}

void driver_exit(void) {
	printk("unloading driver module\n");
	proc_remove(my_proc);
	return;
}

module_init(driver_init);
module_exit(driver_exit);

MODULE_LICENSE("GPL");
