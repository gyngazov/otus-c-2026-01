#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void m(uint64_t *data);
uint64_t *add_element(int x, uint64_t *y);
void print_int(uint64_t val);
uint64_t p(uint64_t val);
void f(uint64_t *rv, int val);

int main (int argc, char **argv)
{
    int data[] = {4, 8, 15, 16, 23, 42};
    const char *empty_str = "\0";
    const int data_length = sizeof(data) / sizeof(int);

    uint64_t *rv;
    rv = NULL;
    for (int i = 0; i < data_length; i++)
        rv = add_element(data[i], rv);
    m(rv);
    exit(0);
    puts(empty_str);
    f(rv, 0);
    m(rv);
    puts(empty_str);
    return 0;
}

void m(uint64_t *data)
{
    if (data == NULL)
        return;
    print_int(*data);
    m(data + 8);
}

uint64_t *add_element(int x, uint64_t *y)
{
    uint64_t *res;
    res = (uint64_t *) malloc(16);
    if (res == NULL)
        abort();
    *res = x;
    res += 8;
    res = y;
    return res - 8;
}

void print_int(uint64_t val)
{
    const char *int_format = "%ld \0";
    printf(int_format, val);
    fflush(stdin);
}

uint64_t p(uint64_t val)
{
    return val & 1;
}

void f(uint64_t *rv, int val)
{
    if (rv == NULL)
        return;
    uint64_t res = p(*rv);
    if (res != 0) 
        rv = add_element(val, rv);
    f(rv + 8, val);
}
