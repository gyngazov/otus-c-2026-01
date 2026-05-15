/*
 * lkm_debug.c — простой учебный Linux Kernel Module (LKM)
 *
 * Назначение этого модуля:
 *   1) показать минимальную структуру загружаемого модуля ядра;
 *   2) дать удобные точки входа для отладки и трассировки;
 *   3) продемонстрировать, как userspace взаимодействует с kernelspace;
 *   4) предоставить «медленный» участок кода, который хорошо видно
 *      в perf, ftrace и bpftrace.
 *
 * Важное техническое замечание (о пробе kprobe на init/exit):
 *   Макросы __init и __exit помещают функции в специальные секции ядра.
 *   После загрузки модуля память этих функций освобождается, поэтому
 *   kprobe на них невозможен — зацепиться некуда.
 *   Решение: убраны атрибуты __init/__exit, а вместо них введены
 *   тонкие обёртки lkm_do_init() / lkm_do_exit(), которые остаются
 *   в памяти и доступны для kprobe.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/random.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("lkm_debug");
MODULE_DESCRIPTION("Simple LKM for debugging with perf/ftrace/strace/bpftrace");
MODULE_VERSION("1.1");

#define DEVICE_NAME  "lkm_debug"
#define PROC_NAME    "lkm_debug"
#define IOCTL_MAGIC  'D'
#define IOCTL_PING   _IO(IOCTL_MAGIC, 1)
#define IOCTL_SLOW   _IO(IOCTL_MAGIC, 2)

/* ------------------------------------------------------------------ */
/* Вспомогательная «медленная» функция                                 */
/* ------------------------------------------------------------------ */
/*
 * noinline запрещает встраивание — функция остаётся отдельным символом,
 * видным в perf report, ftrace function_graph и bpftrace kprobe.
 *
 * volatile acc препятствует агрессивной оптимизации цикла компилятором.
 */
static noinline void lkm_slow_work(unsigned int iterations)
{
	unsigned int i;
	volatile unsigned long acc = 0;

	for (i = 0; i < iterations; i++)
		acc += i * i;

	pr_debug("%s: slow_work acc=%lu\n", DEVICE_NAME, acc);
}

/* ------------------------------------------------------------------ */
/* /proc/lkm_debug                                                      */
/* ------------------------------------------------------------------ */
static int lkm_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "lkm_debug: calling slow_work()\n");
	lkm_slow_work(100000);
	seq_printf(m, "lkm_debug: done\n");
	return 0;
}

static int lkm_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, lkm_proc_show, NULL);
}

static const struct proc_ops lkm_proc_ops = {
	.proc_open    = lkm_proc_open,
	.proc_read    = seq_read,
	.proc_lseek   = seq_lseek,
	.proc_release = single_release,
};

/* ------------------------------------------------------------------ */
/* misc device — /dev/lkm_debug                                        */
/* ------------------------------------------------------------------ */
static long lkm_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case IOCTL_PING:
		pr_info("%s: IOCTL_PING received\n", DEVICE_NAME);
		return 0;
	case IOCTL_SLOW:
		pr_info("%s: IOCTL_SLOW — running slow_work\n", DEVICE_NAME);
		lkm_slow_work(500000);
		return 0;
	default:
		return -ENOTTY;
	}
}

static int lkm_open(struct inode *inode, struct file *file)
{
	pr_debug("%s: device opened\n", DEVICE_NAME);
	return 0;
}

static int lkm_release(struct inode *inode, struct file *file)
{
	pr_debug("%s: device closed\n", DEVICE_NAME);
	return 0;
}

static ssize_t lkm_write(struct file *f, const char __user *buf,
			  size_t len, loff_t *off)
{
	char kbuf[64] = {};
	size_t copy_len = min(len, sizeof(kbuf) - 1);

	if (copy_from_user(kbuf, buf, copy_len))
		return -EFAULT;

	pr_info("%s: write: %.*s\n", DEVICE_NAME, (int)copy_len, kbuf);
	return len;
}

static const struct file_operations lkm_fops = {
	.owner          = THIS_MODULE,
	.open           = lkm_open,
	.release        = lkm_release,
	.write          = lkm_write,
	.unlocked_ioctl = lkm_ioctl,
};

static struct miscdevice lkm_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = DEVICE_NAME,
	.fops  = &lkm_fops,
};

/* ------------------------------------------------------------------ */
/* Обёртки для bpftrace/ftrace                                         */
/* ------------------------------------------------------------------ */
static noinline int lkm_do_init(void)
{
	int ret;

	ret = misc_register(&lkm_misc);
	if (ret) {
		pr_err("%s: misc_register failed: %d\n", DEVICE_NAME, ret);
		return ret;
	}

	if (!proc_create(PROC_NAME, 0444, NULL, &lkm_proc_ops)) {
		pr_err("%s: proc_create failed\n", DEVICE_NAME);
		misc_deregister(&lkm_misc);
		return -ENOMEM;
	}

	pr_info("%s: module loaded. /dev/%s  /proc/%s\n",
		DEVICE_NAME, DEVICE_NAME, PROC_NAME);
	return 0;
}

static noinline void lkm_do_exit(void)
{
	remove_proc_entry(PROC_NAME, NULL);
	misc_deregister(&lkm_misc);
	pr_info("%s: module unloaded\n", DEVICE_NAME);
}

/* ------------------------------------------------------------------ */
/* Точки входа/выхода модуля                                       */
/* ------------------------------------------------------------------ */
static int lkm_debug_init(void)
{
	return lkm_do_init();
}

static void lkm_debug_exit(void)
{
	lkm_do_exit();
}

module_init(lkm_debug_init);
module_exit(lkm_debug_exit);
