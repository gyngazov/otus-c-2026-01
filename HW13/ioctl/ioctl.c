#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "ioctl.h"

#define DEV "/dev/"DEVICE_NAME

int main() {
    char *err = "";
    int ret = EXIT_FAILURE;

    int fd = open(DEV, O_RDWR);
    if (fd < 0) {
        err = "Failed to open device";
        goto end;
    }

    int send_val = 123;
    int recv_val = 0;

    if (ioctl(fd, IOCTL_SET_VAL, &send_val) == -1) {
        err = "ioctl set failed";
        goto end;
    }

    if (ioctl(fd, IOCTL_GET_VAL, &recv_val) == -1) {
        err = "ioctl get failed";
        goto end;
    }
    ret = EXIT_SUCCESS;
    printf("App: Received value from kernel: %d\n", recv_val);

    end:
    perror(err);
    if (fd > 0)
        close(fd);
    return ret;
}



