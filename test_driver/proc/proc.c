#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define PROC_NAME "proc_rw"

struct user_item {
	char *kbuf; /*用于保存用户传下来的字符串*/
	struct list_head list;  
};

LIST_HEAD(head);

static int demo_read(char *page, char **start, off_t off,
			  int count, int *eof, void *data)
{
	int ret = 0;
	struct user_item *pos;

	list_for_each_entry (pos, &head, list) {
		ret += sprintf(page+ret, "%s", pos->kbuf);		
	}

	return ret;
}

static int demo_write(struct file *file, const char __user *buffer,
			   unsigned long count, void *data)
{
	struct user_item *item = kzalloc(sizeof(struct user_item), GFP_KERNEL);
	if (NULL == item) {
		return -ENOMEM;
	}

	item->kbuf = kzalloc(count + 1, GFP_KERNEL);
	if (NULL == item->kbuf) {
		goto error0;
	}

	if (copy_from_user(item->kbuf, buffer, count)) {
		goto error1;
	}

	item->kbuf[count] = '\0';

	list_add_tail(&item->list, &head);

	return count;
error1:
	kfree(item->kbuf);
error0:	
	kfree(item);
	return -ENOMEM;
}

static int __init demo_init(void)
{
	struct proc_dir_entry *proc;

	proc = create_proc_entry(PROC_NAME, 00644, NULL);
	if (NULL == proc) {
		return -ENOMEM;
	}
	proc->read_proc = demo_read;
	proc->write_proc = demo_write;

	return 0;
}

static void __exit demo_exit(void)
{
	struct user_item *pos, *n;

	list_for_each_entry_safe(pos, n, &head, list) {
		kfree(pos->kbuf);
		kfree(pos);
	}
	remove_proc_entry(PROC_NAME,NULL);
}

module_init(demo_init);
module_exit(demo_exit);

MODULE_AUTHOR("millet9527");
MODULE_VERSION("mi plus 2017");
MODULE_DESCRIPTION("It is a demo for driver module");
MODULE_LICENSE("GPL");
