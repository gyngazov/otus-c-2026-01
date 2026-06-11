#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define DEV "/dev/ring_buffer"

int main() {
    char *err = "";
    int ret;

    int fd = open(DEV, O_RDWR);
    if (fd < 0) {
        err = "Failed to open device";
        ret = EXIT_FAILURE;
        goto end;
    }

    char buf[] = "ring buffer record 100";
    if (write(fd, buf, strlen(buf)) == -1) {
        err = "write error";
        ret = errno;
        goto end;
    } else {
        puts("written to rb");
    }
    char buff[256];
    if (read(fd, buff, 256) == -1) {
        err = "read error";
        ret = errno;
        goto end;
    } else {
        printf("rb line: %s\n", buff);
    }

    ret = EXIT_SUCCESS;

    end:
    
    if (fd > 0) {
        if (close(fd) == -1) {
            ret = errno;
            err = "close error";
        }
    }
    perror(err);
    return ret;
}