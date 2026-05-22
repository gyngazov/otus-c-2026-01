#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

#define TMP "/tmp/1"

void del_test()
{
    unlink(TMP);
}

int cre_test()
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    return creat(TMP, mode);
}

void main()
{
    del_test();
    cre_test();
}

