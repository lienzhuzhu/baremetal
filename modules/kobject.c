#include <linux/init.h>
#include <linux/module.h>

#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/sched.h>

#define MAX_SIZE 1024
#define BCM2837_GPIO_ADDRESS 0x3f200000


static char data_buffer[MAX_SIZE + 1] = {0};
static unsigned int *gpio_registers = NULL;
static char *led_state; // the value within the file
static struct kobject *led_state_kobj;


/*
 * LED control functions
 */
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


/*
 * The LED file to write and read data to and from
 */
static ssize_t led_state_show(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
	return sprintf(buf, "%s\n", led_state);
}

//TODO
ssize_t led_state_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    unsigned int pin = UINT_MAX;
	unsigned int value = UINT_MAX;

    sscanf(buf, "%s", &led_state);

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


/* sysfs attribute struct for led_state */
static struct kobj_attribute led_state_attribute = {
    __ATTR(led_state, 0664, led_state_show, led_state_store)
};


static int driver_init(void) {
    int ret;

    printk("loading driver module\n");

    /* 
     * create a directory under /sys/kernel/
     * full path to this subdirectory is /sys/kernel/led_state_dir
     */
    led_state_kobj = kobject_create_and_add("led_state_dir", kernel_kobj);
	if (!led_state_kobj) {
		return -ENOMEM;
    }

	/* Create the file associated with this kobject */
	ret = sysfs_create_file(led_state_kobj, &led_state_attribute.attr);
	if (ret != 0) {
		kobject_put(led_state_kobj); 
    }

	return ret;
}

static void driver_exit(void) {
    printk("unloading driver module\n");
    kobject_put(led_state_kobj);
    return;
}

moudle_init(driver_init);
module_exit(driver_exit);

MODULE_LICENSE("GPL");
MODULE_LICENSE("Lien Zhu");