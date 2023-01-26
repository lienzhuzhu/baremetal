#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/proc_fs.h>
#include <linux/slab.h>

#include <asm/io.h>

#include <linux/fs.h>

#define MAX_SIZE 1024
#define BCM2837_GPIO_ADDRESS 0x3f200000

static struct proc_dir_entry *my_proc = NULL;

static char data_buffer[MAX_SIZE + 1] = {0};

static unsigned int *gpio_registers = NULL;


static void gpio_pin_on(unsigned int pin) {

	unsigned int fsel_index = pin / 10; // index into function select register
	unsigned int fsel_bitpos = pin % 10; // index into bit within function select register
	unsigned int *gpio_fsel = gpio_registers + fsel_index; // get address of fsel
	unsigned int *gpio_on_register = (unsigned int *)( (char *)gpio_registers + 0x1c );

	*gpio_fsel &= ~(7 << (fsel_bitpos * 3)); // clear fsel
	*gpio_fsel |= (1 << (fsel_bitpos * 3)); // set fsel to 0b001 to establish this pin as OUTPUT
	*gpio_on_register |= (1 << pin); // turn pin on

	return;
}

static void gpio_pin_off(unsigned int pin) {
	unsigned int *gpio_off_register = (unsigned int *)( (char *)gpio_registers + 0x28 );
	*gpio_off_register |= (1 << pin);
	return;
}

ssize_t driver_read(struct file *f, char __user *user, size_t size, loff_t *off) {
	//if (copy_to_user(user, "Hello!\n", 7) != 0) {
	//	printk("copy_to_user() failed\n");
	//	return -1;
	//}

	//if (*off != 0) {
	//	return 0; // an effort to stop infinite Hello!'s
	//}
	//return 7;

	return simple_read_from_buffer(user, size, off, "Hello!\n", 7);

}

ssize_t driver_write(struct file *f, const char __user *user, size_t size, loff_t *off) {

	unsigned int pin = UINT_MAX;
	unsigned int value = UINT_MAX;

	memset(data_buffer, 0x0, sizeof(data_buffer));

	if (size > MAX_SIZE) {
		size = MAX_SIZE;
	}

	if (copy_from_user(data_buffer, user, size) != 0) {
		printk("copy_from_user() failed\n");
		return -1;
	}

	printk("Data buffer: %s\n", data_buffer);

	if (sscanf(data_buffer, "%d,%d", &pin, &value) != 2) {
		printk("Incorrect input format\n");
		return size;
	}

	if (pin > 21 || pin < 0) {
		printk("invalid pin number\n");
		return size;
	}

	if (value != 0 && value != 1) {
		printk("invalid control value\n");
		return size;
	}

	printk("pin provided: %d, value: %d\n", pin, value);
	if (value == 1) {
		gpio_pin_on(pin);
	} else {
		gpio_pin_off(pin);
	}

	return size;
}

static const struct proc_ops fops = {
	.proc_read = driver_read,
	.proc_write = driver_write,
};


int driver_init(void) {
	printk("loaded driver module\n");

	// request virtual address for GPIO pins physical address
	gpio_registers = (int *)ioremap(BCM2837_GPIO_ADDRESS, PAGE_SIZE);
	if (gpio_registers == NULL ){
		printk("failed to map GPIO memory\n");
		return -1;
	}
	printk("successfully mapped GPIO memory\n");

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
	iounmap(gpio_registers);
	proc_remove(my_proc);
	return;
}

module_init(driver_init);
module_exit(driver_exit);

MODULE_LICENSE("GPL");
