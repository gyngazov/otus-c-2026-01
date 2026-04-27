#include <stdio.h>

struct S {
    int n;
};

void f(struct S *s)
{
    (s->n)++;
}

void main()
{
    struct S x;
    x.n = 5;
    f(&x);
    printf("%d\n", x.n);
}

