#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/delay.h>

/*
 	非睡眠短延时：
		ndelay,udelay,mdelay
	睡眠短延时：
		msleep, ssleep
		msleep_interruptible
 */

/*
 	time_before/time_after
	time_before_eq/time_after_eq
 */

/*
 	获取内核时间的接口：

	current_kernel_time();
	do_gettimeofday();
 */

void mysleep(int s)
{
	wait_queue_head_t wait;

	init_waitqueue_head(&wait);

	wait_event_interruptible_timeout(wait, 0, s*HZ);
}

/*非睡眠长延时*/
void mydelay(int s)
{
	unsigned long expires = jiffies + s*HZ;

	while (time_before(jiffies, expires)) {
		schedule();	
	}
}

void display_time(void)
{
	u32 s = jiffies/HZ;
	int h, m;

	h = s/3600;
	m = s%3600/60;
	s = s%60;

	printk("%02dh-%02dm-%02ds\n", h, m, s);
}

static int __init demo_init(void)
{
	struct timespec t0, t1;
	int us;

	t0 = current_kernel_time();
	mdelay(7);
	t1 = current_kernel_time();

	us = (t1.tv_sec - t0.tv_sec)*1000000 + (t1.tv_nsec - t0.tv_nsec)/1000;

	printk("current_kernel_time: %dus\n", us);

	return 0;
}
module_init(demo_init);

static void __exit demo_exit(void)
{
}
module_exit(demo_exit);


MODULE_LICENSE("GPL");


