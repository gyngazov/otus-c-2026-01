
#ifndef MY_IOCTL_H
#define MY_IOCTL_H

#include <linux/ioctl.h>

#define MY_MAGIC        'Q'
#define IOCTL_GET_VAL   _IOR(MY_MAGIC, 1, int*)
#define IOCTL_SET_VAL   _IOW(MY_MAGIC, 2, int*)

#endif

#define DEVICE_NAME     "int_dev"
#define PROC_NAME        DEVICE_NAME"_proc"