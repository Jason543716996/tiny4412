#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

static struct tiny4412_key {
	int irqnum;
	char *name;
	int cnt;
}keys[] = {
	{IRQ_EINT(26), "key1", 0},
	{IRQ_EINT(27), "key2", 0},
	{IRQ_EINT(28), "key3", 0},
	{IRQ_EINT(29), "key4", 0}
};

/*定义一个struct work_struct类型的一个变量，对应中断的下半部任务*/
static struct keywork {
	struct work_struct work;
	struct tiny4412_key *pdev;
}keywork;

static void do_4key_bh(struct work_struct *work)
{
	struct keywork *p = container_of(work, struct keywork, work);

	p->pdev->cnt++;
	printk("%s is %s.\n", p->pdev->name, (p->pdev->cnt%2)?"down":"up");

	if (in_interrupt()) {
		printk("In interrupt updown text..\n");
	} else {
		printk("In process updown text...\n");
	}
}

/*上半部处理函数： 中断所注册的中断处理函数，中断发生后由内核自动调用*/
static irqreturn_t do_4keys(int irqnum, void *data)
{
	/*给下半部任务对象的处理函数的点心变量赋值*/
	keywork.pdev = data;

	/*将中断下半部任务对象交给调度器调度*/
	schedule_work(&keywork.work);

	printk("schedule ...top...\n");

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
	/*初始化中断下半部任务对象*/
	INIT_WORK(&keywork.work, do_4key_bh);

	return register_4keys();
}
module_init(demo_init);

static void __exit demo_exit(void)
{
	flush_work(&keywork.work);

	unregister_4keys();
}
module_exit(demo_exit);


MODULE_LICENSE("GPL");


