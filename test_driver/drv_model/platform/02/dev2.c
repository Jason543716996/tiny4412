#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>

static void dummy_release(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);	

	printk("%s device is release...\n", pdev->name);
}

static struct platform_device mydev2 = {
	.name	=  "cherry",
	.id	=  1,
	.dev	= {
		.release = dummy_release,
	},
};

module_driver(mydev2, platform_device_register, platform_device_unregister);

MODULE_LICENSE("GPL");
