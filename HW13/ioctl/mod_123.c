
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/random.h>

#include "ioctl.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("otus hw 13");

static int device_value = 0;

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
{
    int __user *user_ptr = (int __user *)arg;
    int val;

    switch (cmd) {
        case IOCTL_SET_VAL:
            if (copy_from_user(&val, user_ptr, sizeof(int)) != 0)
                return -EFAULT;
            device_value = val;
            pr_info("Module: Value set to %d\n", device_value);
            break;

        case IOCTL_GET_VAL:
            val = device_value;
            if (copy_to_user(user_ptr, &val, sizeof(int)) != 0)
                return -EFAULT;
            pr_info("Module: Value sent to user\n");
            break;

        default:
            return -EINVAL;
    }
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = my_ioctl,
};

static struct miscdevice mod_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = DEVICE_NAME,
	.fops  = &fops,
};

static int proc_log(struct seq_file *m, void *v)
{
    seq_printf(m, "open proc done\n");
	return 0;
}

static int proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_log, NULL);
}

static const struct proc_ops mod_proc_ops = {
	.proc_open    = proc_open,
	.proc_read    = seq_read,
	.proc_lseek   = seq_lseek,
	.proc_release = single_release,
};

static int __init my_init(void) 
{

    int ret;

	ret = misc_register(&mod_misc);
	if (ret) {
		pr_err("%s: misc_register failed: %d\n", DEVICE_NAME, ret);
		return ret;
	}

	if (!proc_create(PROC_NAME, 0444, NULL, &mod_proc_ops)) {
		pr_err("%s: proc_create failed\n", DEVICE_NAME);
		misc_deregister(&mod_misc);
		return -ENOMEM;
	}

	pr_info("%s: module loaded. /dev/%s  /proc/%s\n",
		DEVICE_NAME, DEVICE_NAME, PROC_NAME);
    
    return 0;
}

static void __exit my_exit(void) 
{
    remove_proc_entry(PROC_NAME, NULL);
	misc_deregister(&mod_misc);
	pr_info("%s: module unloaded\n", DEVICE_NAME);
}

module_init(my_init);
module_exit(my_exit);