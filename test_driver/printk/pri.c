#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

static int __init demo_init(void)
{
	printk("%s ok!\n", __func__);

	return 0;
}

static void __exit demo_exit(void)
{
	printk(KERN_EMERG"%s ok!\n", __func__);
}

module_init(demo_init);
module_exit(demo_exit);

MODULE_AUTHOR("millet9527");
MODULE_VERSION("mi plus 2017");
MODULE_DESCRIPTION("It is a demo for driver module");
MODULE_LICENSE("GPL");
