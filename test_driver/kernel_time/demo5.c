#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/timer.h>

#define SEC 5

/*1st: 实例化一个定时器的对象*/
static struct timer_list timer;

/*4th: 定时时间到后由内核自动调用此函数*/
static void do_timer(unsigned long data)
{
	printk("hello, it's to have lunch...\n");

	mod_timer(&timer, jiffies + SEC*HZ);
}

static int __init demo_init(void)
{
	/*2nd: 初始化定时器对应的未来时间点和定时处理函数及函数需要的实参*/
	setup_timer(&timer, do_timer, 0);
	timer.expires = jiffies + SEC*HZ; 

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


