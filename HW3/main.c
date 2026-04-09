#include <stdio.h>
#include <stdlib.h>

struct Node {
    long val;
    struct Node *next;
};

void print_int(long num)
{
    printf("%ld \0", num);
    fflush(0);
}
// печать от корня
void m(struct Node *node)
{
    if (node == NULL)
        return;
    print_int(node->val);
    m(node->next);
}
// добавление головы к хвосту
struct Node *add_element(long new, struct Node *tail)
{
    struct Node *node;
    node = (struct Node *) malloc(sizeof(struct Node));
    if (node == NULL)
        abort();
    node->val = new;
    node->next = tail;
    return node;
}
// четность
long p(long x)
{
    return x & 1;
}

void f(struct Node *node) // сделать две перем
{
    if (node == NULL)
        return;
    if (p(node->val) != 0L)
        node = add_element(node->val, NULL);
    f(node->next);
}

int main(int argc, char **argv)
{
    long data[] = {4L, 8L, 15L, 16L, 23L, 42L};
    int data_length = sizeof(data) / 8;
    struct Node *node = NULL;
    for (int i = data_length - 1; i >= 0; i--)
        node = add_element(data[i], node);
    struct Node *start = node;
    puts("\0");
    m(node);
    f(start);
    m(start);
    puts("\0");
    return 0;
}