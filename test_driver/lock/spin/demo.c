#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>

/*以上头文件所在的路劲为include/linux/...*/

#define DEVNAME "spin"

static int kcnt = 1;

#define CNT 5

static spinlock_t spin;

static int  demo_open (struct inode *inodp, struct file *filp)
{
	return 0;
}

static void print_kcnt_str(const char *str)
{
	int i;

	spin_lock(&spin);
	for (i = 0; i < CNT; i++) {
		printk("%s	-->   %d\n", str, kcnt);
		kcnt++;
		mdelay(50);
	}
	spin_unlock(&spin);
}

static long demo_unlocked_ioctl (struct file *filp, unsigned int request, 
								unsigned long arg)
{
	print_kcnt_str((char *)arg);

	return 0;
}

static int  demo_release (struct inode *inodp, struct file *filp)
{
	return 0;
}

static struct file_operations fops = {
	.open		=	demo_open,
	.unlocked_ioctl	=	demo_unlocked_ioctl,
	.release	=	demo_release,
};

static struct miscdevice  misc = {
	.minor	=  MISC_DYNAMIC_MINOR, /*动态由内核分配一个次设备号， 主设备号默认为10*/
	.name	=  DEVNAME, /*指定设备文件名*/
	.fops	=  &fops, /*指定和设备文件关联的驱动集合*/
};

static int __init demo_init(void)
{
	spin_lock_init(&spin);

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
