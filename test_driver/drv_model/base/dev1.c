#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/interrupt.h>

#include "mydev.h"

extern struct bus_type mibus;

/*设备对象被移除时刻内核将自动调用此函数*/
static void    mi_release(struct device *dev)
{
	mydev_t *pdev = container_of(dev, mydev_t, dev);

	printk("%s device is release...\n", pdev->name);
}

static mydev_t mydev1 = {
	.dev = {
		.init_name = "sun1",	
		.bus	   = &mibus,
		.release   = mi_release,
	},
	.name	= "sun1",
};

static int __init mydev1_init(void)
{
	return device_register(&mydev1.dev);
}

module_init(mydev1_init);

static void __exit mydev1_exit(void)
{
	device_unregister(&mydev1.dev);
}
module_exit(mydev1_exit);

MODULE_LICENSE("GPL");
