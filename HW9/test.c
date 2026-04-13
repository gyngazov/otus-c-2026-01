#include <stdio.h>

void main()
{
    FILE *fd;
    fd = fopen("/tmp/file.bin", "r");
    fseek(fd, 0L, SEEK_END);
    printf("%ld\n", ftell(fd));
}