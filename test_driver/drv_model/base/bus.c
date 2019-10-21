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

/*
   总线是用于匹配设备和驱动的，match为匹配规则
   返回1则表示匹配成功，0则失败。	
 */
static int mymatch(struct device *dev, struct device_driver *drv)
{
	mydev_t *pdev = container_of(dev, mydev_t, dev);

	printk("1st: try match %s device and %s driver.\n",
			pdev->name, drv->name);

	return !strncmp(drv->name, pdev->name, strlen(drv->name));
}

/*驱动模型中一个总线对应如下一个结构体类型的变量*/
static struct bus_type mibus = {
	.name	= "myplatform",	
	.match  = mymatch,
};
EXPORT_SYMBOL_GPL(mibus);


module_driver(mibus, bus_register, bus_unregister);

MODULE_LICENSE("GPL");
