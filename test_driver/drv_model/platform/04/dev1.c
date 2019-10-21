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

static struct resource	reses[] = {
	{
		.start	= IRQ_EINT(26),
		.end	= IRQ_EINT(29),
		.name	= "key",
		.flags	= IORESOURCE_IRQ
	},
	
	{
		.start	= 0x10060000,
		.end	= 0x1006000f,
		.name	= "wdt",
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device mydev1 = {
	.name	=  "peach",
	.id	=  -1,
	.dev	= {
		.release = dummy_release,
	},
	.num_resources = ARRAY_SIZE(reses),
	.resource      = reses, 
};

//module_platform_driver(mydev1);
module_driver(mydev1, platform_device_register, platform_device_unregister);

MODULE_LICENSE("GPL");
