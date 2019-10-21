#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/interrupt.h>

#define DEVNAME "key"

static struct tiny4412_key {
	int num;
	int irqnum;
	char *name;
	int cnt;
}keys[] = {
	{0, IRQ_EINT(26), "key1", 0},
	{1, IRQ_EINT(27), "key2", 0},
	{2, IRQ_EINT(28), "key3", 0},
	{3, IRQ_EINT(29), "key4", 0}
};

/*定义一个struct tasklet_struct类型的一个变量，对应中断的下半部任务*/
static struct tasklet_struct task;

/*定义一个用于实现进程阻塞的对象*/
static wait_queue_head_t wait;

/*此变量用于阻塞条件的检查*/
static int dnup_flg = 0;

static char kbuf[4] = {'0', '0', '0', '0'};

/*内核定时器用于实现按键的消抖动*/
static struct timer_list timer;

static ssize_t 
key_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	if (cnt != 4) {
		return -EINVAL;
	}	

	/*判断用户是否按下或抬起按键，没有动作阻塞*/
	wait_event_interruptible(wait, dnup_flg != 0);

	if (copy_to_user(buf, kbuf, 4)) {
		return -EINVAL;
	}

	dnup_flg = 0;

	return cnt;
}

static struct file_operations fops = {
	.read	=   key_read,
};

/*对应一个混杂设备驱动对象*/
static struct miscdevice misc = {
	.minor	=  MISC_DYNAMIC_MINOR,
	.name	=  DEVNAME,
	.fops	=  &fops,
};

/*定时处理函数*/
static void do_timer(unsigned long data)
{
	int index;

	struct tiny4412_key *pdev = (void *)data;	
	pdev->cnt++;
	
	index = pdev->num;

	kbuf[index] = pdev->cnt%2 + '0';
	dnup_flg = 1;
	wake_up_interruptible(&wait);
}

/*中断下半部处理函数*/
static void do_4key_bh(unsigned long data)
{
	mod_timer(&timer, jiffies + msecs_to_jiffies(30));
	timer.data = data;
}

/*上半部处理函数： 中断所注册的中断处理函数，中断发生后由内核自动调用*/
static irqreturn_t do_4keys(int irqnum, void *data)
{
	/*给下半部任务对象的处理函数的点心变量赋值*/
	task.data = (unsigned long)data;

	/*将中断下半部任务对象交给调度器调度*/
	tasklet_schedule(&task);

	return IRQ_HANDLED;
}

static int register_4keys(void)
{
	int i = 0;
	int ret;

	for (; i < ARRAY_SIZE(keys); i++) {
		ret = request_irq(keys[i].irqnum, do_4keys, 
				IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
				keys[i].name, keys+i
		      );
		if (ret < 0) {
			goto error;
		}
	}


	return 0;
error:
	while (i--) {
		free_irq(keys[i].irqnum, keys+i);
	}

	return ret;
}

static void unregister_4keys(void)
{
	int i = ARRAY_SIZE(keys);

	while (i--) {
		free_irq(keys[i].irqnum, keys+i);
	}
}

static int __init demo_init(void)
{
	int ret;

	/*初始化中断下半部任务对象*/
	tasklet_init(&task, do_4key_bh, 0);

	ret = misc_register(&misc);
	if (ret < 0) {
		return ret;
	}

	ret = register_4keys();
	if (ret < 0) {
		goto error0;
	}

	init_waitqueue_head(&wait);

	/*初始化内核定时器*/
	setup_timer(&timer, do_timer, 0);

	return 0;
error0:
	misc_deregister(&misc);
	return ret;
}
module_init(demo_init);

static void __exit demo_exit(void)
{
	tasklet_kill(&task);
	unregister_4keys();
	misc_deregister(&misc);
	del_timer_sync(&timer);
}
module_exit(demo_exit);

MODULE_LICENSE("GPL");
