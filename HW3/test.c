#include <stdio.h>

void main()
{
    long data[] = {4, 8, 15, 16, 23, 42};
    printf("i: %d, d[i]: %ld, size: %d\n\n", 3, data[3], sizeof(data));
    printf("%ld\n", sizeof(long));
}