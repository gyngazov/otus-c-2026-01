#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "my_ioctl.h"

int main() {
    int fd = open("/dev/my_device", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    int send_val = 42;
    int recv_val = 0;

    /* Передача значения в ядро */
    if (ioctl(fd, IOCTL_SET_VAL, &send_val) == -1) {
        perror("ioctl set failed");
    }

    /* Чтение значения из ядра */
    if (ioctl(fd, IOCTL_GET_VAL, &recv_val) == -1) {
        perror("ioctl get failed");
    } else {
        printf("App: Received value from kernel: %d\n", recv_val);
    }

    close(fd);
    return 0;
}
// header
#ifndef MY_IOCTL_H
#define MY_IOCTL_H

#include <linux/ioctl.h>

/* Магический номер, тип команды (код) и тип аргумента (int) */
#define MY_MAGIC 'k'
#define IOCTL_GET_VAL _IOR(MY_MAGIC, 1, int*)
#define IOCTL_SET_VAL _IOW(MY_MAGIC, 2, int*)

#endif


// kernel 
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "my_ioctl.h"

MODULE_LICENSE("GPL");

static int major_number;
static int device_value = 0;

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    int __user *user_ptr = (int __user *)arg;
    int val;

    switch (cmd) {
        case IOCTL_SET_VAL:
            /* Копируем данные из пользовательского пространства в ядро */
            if (copy_from_user(&val, user_ptr, sizeof(int)) != 0)
                return -EFAULT;
            device_value = val;
            pr_info("Module: Value set to %d\n", device_value);
            break;

        case IOCTL_GET_VAL:
            /* Копируем данные из ядра в пользовательское пространство */
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

static int __init my_init(void) {
    major_number = register_chrdev(0, "my_device", &fops);
    if (major_number < 0) return major_number;
    pr_info("Module: registered with major %d\n", major_number);
    return 0;
}

static void __exit my_exit(void) {
    unregister_chrdev(major_number, "my_device");
    pr_info("Module: unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

