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

static int myprobe (struct platform_device *pdev)
{
	printk("%s device match ok %s driver.\n",
			pdev->name, pdev->dev.driver->name);

	return 0;
}

static int myremove (struct platform_device *pdev)
{
	printk("%s device remove ok %s driver.\n",
			pdev->name, pdev->dev.driver->name);

	return 0;
}

static struct platform_driver mydrv = {
	.probe	= myprobe,
	.remove	= myremove,
	.driver	= {
		.name	=  "cherry",
	}
};

module_platform_driver(mydrv);

MODULE_LICENSE("GPL");
