#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/timer.h>

#define SEC 5

void do_timer(unsigned long data);

DEFINE_TIMER(timer, do_timer, SEC*HZ, 0);

/*4th: 定时时间到后由内核自动调用此函数*/
void do_timer(unsigned long data)
{
	printk("hello, it's to have lunch...\n");

	mod_timer(&timer, jiffies + SEC*HZ);
}


static int __init demo_init(void)
{
	timer.expires += jiffies;

	/*3rd: 向内核注册并启动定时器*/
	add_timer(&timer);

	return 0;
}
module_init(demo_init);

static void __exit demo_exit(void)
{
	del_timer_sync(&timer);
}
module_exit(demo_exit);

MODULE_LICENSE("GPL");
