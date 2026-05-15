#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define TMP "/tmp/1"

void del_test()
{
    unlink(TMP);
}

void cre_test()
{
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    close(creat(TMP, mode));
}

void main()
{
    del_test();
    cre_test();
}

