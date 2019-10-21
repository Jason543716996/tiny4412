#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <plat/adc.h>
#include <linux/uaccess.h>
#include <linux/pwm.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>

struct reslocator{
	struct s3c_adc_client *client;
	int channel;
}reslocator = {
	.channel = 0,
};

static ssize_t 
res_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	int result;

	if (cnt != 4) {
		return -EINVAL;
	}

	result = s3c_adc_read(reslocator.client,  reslocator.channel);
	if (result < 0) {
		return -EIO;
	}


	if (put_user(result, (int *)buf)) {
		return -EINVAL;	
	}

	return cnt;
}

static struct file_operations fops = {
	.read	= res_read,
};

static struct miscdevice misc = {
	.minor	=  MISC_DYNAMIC_MINOR,
	.name	=  "reslocator",
	.fops	=  &fops,
};

static int res_probe(struct platform_device *pdev)
{
	int ret;

	reslocator.client = s3c_adc_register(pdev, NULL, NULL, 0);
	if (IS_ERR(reslocator.client)) {
		ret = PTR_ERR(reslocator.client);
		return ret;
	}

	ret = misc_register(&misc);
	if (ret < 0) {
		goto error0;
	}

	return 0;
error0:
	s3c_adc_release(reslocator.client);

	return ret;
}

static int res_remove(struct platform_device *pdev)
{
	s3c_adc_release(reslocator.client);
	misc_deregister(&misc);

	return 0;
}

static struct platform_driver resdrv = {
	.probe	=  res_probe,
	.remove	=  res_remove,
	.driver	=  {
		.name	=  "reslocator",
	},
};

module_platform_driver(resdrv);

MODULE_LICENSE("GPL");


