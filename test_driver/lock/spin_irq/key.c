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

/*4个按键的中断处理函数*/
static irqreturn_t do_keys(int irq, void *data)
{
	struct t4412key_t *pkey = data;
	char kbuf[SZ_64];
	
	pkey->cnt++;
	sprintf(kbuf, "%s is %s!", pkey->name, (pkey->cnt%2)?"down":"up");
	print_kcnt_str(kbuf);

	mdelay(50);

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
