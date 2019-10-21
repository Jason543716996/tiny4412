#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>

/*以上头文件所在的路劲为include/linux/...*/

#define DEVNAME "demo_misc"

/*用于表示设备内部数据的是否满*/
static bool is_full = false;

/*定义一个信号量的锁对象*/
static struct semaphore sema;

static ssize_t demo_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	ssize_t ret;

	if (cnt == 0) {
		return -EINVAL;
	}

	down(&sema);
	if (is_full == true) {
		printk("[kernel]: reading...\n");
		mdelay(50);
		is_full = false;
		printk("[kernel]: finish read...\n");
		ret = cnt;
	} else {
		ret = -EBUSY;
	}
	up(&sema);

	return ret;
}

static ssize_t demo_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *fpos)
{
	ssize_t ret;

	if (cnt == 0) {
		return -EINVAL;
	}

	down(&sema);
	if (is_full == false) {
		printk("[kernel]: writing...\n");
		mdelay(50);
		is_full = true;
		printk("[kernel]: finish write...\n");
		ret = cnt;
	} else {
		ret = -EBUSY;
	}
	up(&sema);

	return ret;
}
	
static struct file_operations fops = {
	.read	=	demo_read,
	.write	=	demo_write,
};

static struct miscdevice  misc = {
	.minor	=  MISC_DYNAMIC_MINOR, /*动态由内核分配一个次设备号， 主设备号默认为10*/
	.name	=  DEVNAME, /*指定设备文件名*/
	.fops	=  &fops, /*指定和设备文件关联的驱动集合*/
};

static int __init demo_init(void)
{
	/*初始化信号量对应的锁*/
	sema_init(&sema, 1);

	/*混杂设备驱动的注册方法, 注册一个字符设备驱动*/
	return misc_register(&misc);
}

static void __exit demo_exit(void)
{
	misc_deregister(&misc);
}

module_init(demo_init);
module_exit(demo_exit);

MODULE_AUTHOR("millet9527");
MODULE_VERSION("mi plus 2017");
MODULE_DESCRIPTION("It is a demo for driver module");
MODULE_LICENSE("GPL");
