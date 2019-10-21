#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/slab.h>
#include <plat/ft5x0x_touch.h>

struct ft5206_pri {
	struct i2c_client *cli;
	struct input_dev *tsdev;
	struct work_struct work;
	int gpio;
	int irqnum;
};

/*中断的上半部处理函数*/
static irqreturn_t do_ft5206_top(int irqnum, void *data)
{
	struct ft5206_pri *pdev = data;

	schedule_work(&pdev->work);	

	disable_irq_nosync(pdev->irqnum);

	return IRQ_HANDLED;
}

/*中断的下半部处理函数*/
static void do_ft5206_bh(struct work_struct *work)
{
	char regval[31];
	int ret;
	int x, y;

	struct ft5206_pri *pdev = container_of(work, struct ft5206_pri, work);
	struct i2c_client *cli = pdev->cli;

	ret = i2c_master_recv(cli, regval, 31);
	if (ret != 31) {
		printk("ERROR: master receive error...\n");
		return;
	}

	/*if down...*/
	if (((regval[3]>>6)&0x3) == 0) {
		x = ((regval[3]&0xf) << 8) | regval[4];	
		y = ((regval[5]&0xf) << 8) | regval[6];	
		input_report_abs(pdev->tsdev, ABS_X, x);
		input_report_abs(pdev->tsdev, ABS_Y, y);
		input_report_abs(pdev->tsdev, ABS_PRESSURE, 1);
		input_report_key(pdev->tsdev, BTN_TOUCH, 1);
	} else if (((regval[3]>>6)&0x3) == 1) {
		input_report_abs(pdev->tsdev, ABS_PRESSURE, 0);
		input_report_key(pdev->tsdev, BTN_TOUCH, 0);
	}

	input_sync(pdev->tsdev);
	enable_irq(pdev->irqnum);
}

static int 
ft5206_probe(struct i2c_client *cli, const struct i2c_device_id *data)
{
	struct ft5206_pri *ts;
	int ret;

	struct ft5x0x_i2c_platform_data * pdata = cli->dev.platform_data;

	ts = kzalloc(sizeof(struct ft5206_pri), GFP_KERNEL);
	if (ts == NULL) {
		return -ENOMEM;
	}

	ts->cli = cli;

	i2c_set_clientdata(cli, ts);

	/*申请触摸屏控制模块所连接的SOC的GPIO，设置为中断专用*/
	ts->gpio = pdata->gpio_irq;
	ret = gpio_request(ts->gpio, "ft5206_eint14");
	if (ret < 0) {
		goto error0;
	}
	s3c_gpio_cfgpin(ts->gpio, pdata->irq_cfg);

	/*注册触摸屏控制模块的中断*/
	ts->irqnum = gpio_to_irq(ts->gpio);
	ret = request_irq(ts->irqnum, do_ft5206_top, IRQF_TRIGGER_FALLING, "FT5206", ts);
	if (ret < 0) {
		goto error1;
	}

	/*为输入设备分配空间*/
	ts->tsdev = input_allocate_device();
	if (IS_ERR(ts->tsdev)) {
		ret = PTR_ERR(ts->tsdev);
		goto error2;
	}

	/*设置事件分类和编码*/
	set_bit(EV_ABS, ts->tsdev->evbit);
	set_bit(EV_KEY, ts->tsdev->evbit);

	set_bit(ABS_X, ts->tsdev->absbit);
	set_bit(ABS_Y, ts->tsdev->absbit);
	set_bit(ABS_PRESSURE, ts->tsdev->absbit);

	set_bit(BTN_TOUCH, ts->tsdev->keybit);

	input_set_abs_params(ts->tsdev, ABS_X, 0, 799, 0, 0);
	input_set_abs_params(ts->tsdev, ABS_Y, 0, 479, 0, 0);
	input_set_abs_params(ts->tsdev, ABS_PRESSURE, 0, 1, 0, 0);

	/*向内核注册输入设备的字符驱动*/
	ret = input_register_device(ts->tsdev);
	if (ret) {
		goto error3;
	}

	/*初始化中断下半部的任务对象*/
	INIT_WORK(&ts->work, do_ft5206_bh);

	return 0;
error3:
	input_free_device(ts->tsdev);
error2:
	free_irq(ts->irqnum, ts);
error1:
	gpio_free(ts->gpio);
error0:
	kfree(ts);
	return ret;
}

static int ft5206_remove(struct i2c_client *cli)
{
	struct ft5206_pri *ts = i2c_get_clientdata(cli);

	input_unregister_device(ts->tsdev);
	free_irq(ts->irqnum, ts);
	gpio_free(ts->gpio);
	kfree(ts);

	return 0;
}

static const struct i2c_device_id tables[] = {
	{"ft5206_ts", },
	{"Shaobin", },
	{}
};

static struct i2c_driver ft5206drv = {
	.probe	=	ft5206_probe,
	.remove	=	ft5206_remove,
	.driver	=	{
		.name	=  "ft5206_ts",
	},
	.id_table = tables,
};

module_driver(ft5206drv, i2c_add_driver, i2c_del_driver);

MODULE_LICENSE("GPL");
