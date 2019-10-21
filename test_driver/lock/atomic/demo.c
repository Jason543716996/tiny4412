#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>

/*以上头文件所在的路劲为include/linux/...*/

#define DEVNAME "atomic"

//static int open_cnt_flg = 1;

/*定义原子类型的变量并初始化其成员值为1*/
static atomic_t open_cnt_flg = ATOMIC_INIT(1);

/*
	static atomic_t open_cnt_flg;
	atomic_set(&open_cnt_flg, 1);
*/

static int demo_open (struct inode *inodp, struct file *filp)
{
#if 0
	int reg;
	reg = open_cnt_flg;
	reg--;
	open_cnt_flg = reg;
	open_cnt_flg--;

	if (open_cnt_flg < 0) {
		open_cnt_flg++;
		return -EBUSY;
	}
#endif
#if 1
	if (!atomic_dec_and_test(&open_cnt_flg)) {
		atomic_inc(&open_cnt_flg);
		return -EBUSY;	
	}
#endif
	return 0;
}

static int demo_release (struct inode *inodp, struct file *filp)
{
	atomic_inc(&open_cnt_flg);
	
	return 0;
}

static struct file_operations fops = {
	.owner		= THIS_MODULE,
	.open		= demo_open,
	.release	= demo_release,
};

static struct miscdevice  misc = {
	.minor	=  MISC_DYNAMIC_MINOR, /*动态由内核分配一个次设备号， 主设备号默认为10*/
	.name	=  DEVNAME, /*指定设备文件名*/
	.fops	=  &fops, /*指定和设备文件关联的驱动集合*/
};

static int __init demo_init(void)
{
	/*初始化信号量对应的锁*/
	mutex_init(&mutex);

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
