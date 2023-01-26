#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

int test_init(void) {
	printk("loaded test module\n");
	return 0;
}

void test_exit(void) {
	printk("unloading test module\n");
	return;
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
