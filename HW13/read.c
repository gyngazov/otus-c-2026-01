#include <linux/fs.h>
#include <linux/file.h>

#define TMP "/tmp/1"

void read_file() {
    const char *path = TMP;
    struct file *file;
    loff_t pos = 0;
    char buf[128];
    int ret;

    file = filp_open(path, O_RDONLY, 0);
    if (IS_ERR(file)) {
        printk(KERN_ERR "Failed to open %s\n", path);
        return;
    }

    ret = kernel_read(file, buf, sizeof(buf) - 1, &pos);
    if (ret >= 0) {
        buf[ret] = '\0';
        printk(KERN_INFO "Read %d bytes: %s\n", ret, buf);
    }

    filp_close(file, NULL);
}