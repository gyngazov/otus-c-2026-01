#include <stdio.h>

struct A {
    int a;
    char b[8];
};

void main()
{
    struct A sa;
    sa.a = 2;
    sa.b[0] = 'C';
    sa.b[1] = '5';
    for (int i = 0; i < sa.a; i++)
        printf("%d %c %p\n", i, sa.b[i], &sa);
}