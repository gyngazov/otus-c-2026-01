#include <stdio.h>
#include <stdlib.h>

void print_int(long num)
{
    printf("%ld \0", num);
    fflush(0);
}

void m(long two)
{
    if (i == 0)
        return;
    print_int(i);
    m(i + 1);
}

long *add_element(long a, long *b)
{
    long *two;
    two = (long *) malloc(2 * sizeof(long));
    if (two == NULL)
        abort();
    *two = a;
    two + 1 = b;
    return two;
}

int main(int argc, char **argv)
{
    long data[] = {4, 8, 15, 16, 23, 42};
    int data_length = sizeof(data) / 8;
    long *two;
    two = NULL;
    for (int i = data_length - 1; i > 0; i--)
        two = add_element(data[i], two);
    
    return 0;
}