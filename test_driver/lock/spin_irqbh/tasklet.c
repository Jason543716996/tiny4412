#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

extern void print_kcnt_str(const char *str);

/*以上头文件所在的路劲为include/linux/...*/
static struct t4412key_t {
	int irq;
	char *name;
	int cnt;
} keys[] = {
	{
		IRQ_EINT(26),"key1", 0
	},
	{
		IRQ_EINT(27),"key2", 0
	},
	{
		IRQ_EINT(28),"key3", 0
	},
	{
		IRQ_EINT(29),"key4", 0
	}
};

/*1st: tasklet机制： 如下一个结构体对象代表中断的下半部任务*/
struct tasklet_struct task;

/*中断的下半部处理函数*/
void do_task_bh(unsigned long data) 
{
	char kbuf[SZ_64];
	struct t4412key_t *pkey = (void *)data;
	
	pkey->cnt++;
	sprintf(kbuf, "[bootm half]: %s is %s!", pkey->name, (pkey->cnt%2)?"down":"up");

	print_kcnt_str(kbuf);
}

/*中断的上半部函数： 注册的中断处理函数*/
static irqreturn_t do_keys(int irq, void *data)
{
	/*给tasklet机制对应对象的点心赋值*/
	task.data = (unsigned long)data;

	/*3rd: 将下半部任务对象交给内核调度器调度*/
	tasklet_schedule(&task);

	return IRQ_HANDLED; 
}

/*自己封装： 注册四个按键中断, 成功返回0， 失败返回错误负值*/
int irq_keys_register(void)
{
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(keys); i++) {
		ret = request_irq(keys[i].irq, do_keys, 
					IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, 
					keys[i].name, keys+i);

		if (ret < 0) {
			goto error0;
		}
	}

	return 0;
error0:
	while (i--) {
		free_irq(keys[i].irq, keys+i);
	}

	return ret;
}

/*自己封装： 移除注册的4个按键中断*/
void irq_keys_unregister(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(keys); i++) {
		free_irq(keys[i].irq, keys+i);
	}
}


