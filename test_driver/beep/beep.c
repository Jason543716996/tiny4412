#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/pwm.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>

#include "ioctl.h"

struct mybeep {
	struct pwm_device *pwm;
	int gpio;
	struct mutex mutex;
	int pwm_id;
}beep = {
	.pwm_id = 0,
};

#define NSPERSEC 1000000000UL

static void beep_set_hz(int hz)
{
	int period = NSPERSEC/hz;

	pwm_config(beep.pwm, period >> 1, period);
	pwm_enable(beep.pwm);

	s3c_gpio_cfgpin(beep.gpio, S3C_GPIO_SFN(0x2));
}

static void beep_mute(void)
{
	pwm_disable(beep.pwm);
	gpio_direction_output(beep.gpio, 0);
}

static long 
beep_unlocked_ioctl (struct file *filp, unsigned int request, unsigned long arg)
{
	switch (request) {
	case BEEP_SET_HZ:
		if (arg > 4000) {
			return -EINVAL;
		}
		beep_set_hz(arg);
	break;
	case BEEP_MUTE:
	default:
		beep_mute();
		break;
	}

	return 0;
}

static int beep_open (struct inode *inodp, struct file *filp)
{
	if (!mutex_trylock(&beep.mutex)) {
		return -EBUSY;
	}

	return 0;
}

static int beep_release (struct inode *inodp, struct file *filp)
{
	mutex_unlock(&beep.mutex);

	return 0;
}

static struct file_operations fops = {
	.open		=  beep_open,
	.unlocked_ioctl	=  beep_unlocked_ioctl,
	.release	=  beep_release,
};

static struct miscdevice misc = {
	.minor	=  MISC_DYNAMIC_MINOR,
	.name	=  "beep",
	.fops	=  &fops,
};

static int beep_probe(struct platform_device *pdev)
{
	/*获取设备资源，注册驱动*/
	struct resource *r;
	int ret;

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	///////////////

	beep.gpio = r->start;

	ret = gpio_request(beep.gpio, "beep");
	if (ret < 0) {
		printk("error!");
		return -EINVAL;
	}

	gpio_direction_output(beep.gpio, 0);

	beep.pwm = pwm_request(beep.pwm_id, "beep");
	if (IS_ERR(beep.pwm)) {
		ret = PTR_ERR(beep.pwm);
		goto error0;
	}

	ret = misc_register(&misc);
	if (ret < 0) {
		goto error1;
	}

	mutex_init(&beep.mutex);
	
	printk("beep insmod is ok!");
	return 0;
error1:
	pwm_free(beep.pwm);
	printk("beep insmod is error!");
error0:
	gpio_free(beep.gpio);
	printk("beep insmod is error!");

	return ret;
}

static int beep_remove(struct platform_device *pdev)
{
	beep_mute();
	pwm_free(beep.pwm);
	gpio_free(beep.gpio);
	misc_deregister(&misc);

	return 0;
}

static struct platform_driver beepdrv = {
	.probe	=  beep_probe,
	.remove	=  beep_remove,
	.driver	=  {
		.name	=  "tiny4412beep",
	},
};

module_platform_driver(beepdrv);

MODULE_LICENSE("GPL");


