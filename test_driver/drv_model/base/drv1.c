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

/*当总线的match返回为1时，内核自动调用匹配成功的驱动对象内部的probe函数*/
static int mydrv_probe (struct device *dev)
{
	mydev_t *pdev = container_of(dev, mydev_t, dev);

	/*资源的申请，驱动的注册等等*/	
	printk("2nd: call %s driver's probe with %s device.\n",
			dev->driver->name, pdev->name);

	return 0;
}

static int mydrv_remove (struct device *dev)
{
	mydev_t *pdev = container_of(dev, mydev_t, dev);

	/*资源的申请，驱动的注册等等*/	
	printk("call %s driver's remove with %s device.\n",
			dev->driver->name, pdev->name);

	return 0;	
}

static struct device_driver mydrv = {
	.name	=  "sun",	
	.bus	=  &mibus,
	.probe	=  mydrv_probe,
	.remove	=  mydrv_remove,
};

module_driver(mydrv, driver_register, driver_unregister);

MODULE_LICENSE("GPL");
