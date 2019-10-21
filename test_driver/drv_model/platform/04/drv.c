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
	struct resource *res;

	printk("%s device match ok %s driver.\n",
			pdev->name, pdev->dev.driver->name);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	printk("start: %#x, end: %#x\n",
			res->start, res->end);

	res = platform_get_resource_byname(pdev, IORESOURCE_IRQ, "key");
	printk("start: %d, end: %d\n", res->start, res->end);

	//platform_get_irq
	//platform_get_irq_byname

	return 0;
}

static int myremove (struct platform_device *pdev)
{
	printk("%s device remove ok %s driver.\n",
			pdev->name, pdev->dev.driver->name);

	return 0;
}

const struct platform_device_id tables[] = {
	{"liqiang", },
	{"peach", },
	{"apple", },
	{"banana", },
	{}   //哨兵
};

static struct platform_driver mydrv = {
	.probe	= myprobe,
	.remove	= myremove,
	.driver	= {
		.name	=  "cherry",
	},
	.id_table = tables,
};

module_platform_driver(mydrv);

MODULE_LICENSE("GPL");
