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

/*中断所注册的中断处理函数，中断发生后由内核自动调用*/
static irqreturn_t do_4keys(int irqnum, void *data)
{
	struct tiny4412_key *pdev = data;	

	pdev->cnt++;
	printk("%s is %s.\n", pdev->name, (pdev->cnt%2)?"down":"up");

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
	return register_4keys();
}
module_init(demo_init);

static void __exit demo_exit(void)
{
	unregister_4keys();
}
module_exit(demo_exit);


MODULE_LICENSE("GPL");


