#include <stdio.h>
#include <stdlib.h>

struct Node {
    long val;
    struct Node *next;
};

// осв. память у списка
void free_nodes(struct Node *node) 
{
    if (node == NULL)
        return;
    free_nodes(node->next);
    free(node);
}

void print_int(struct Node *node)
{
    printf("%ld ", node->val);
    fflush(0);
}

// печать от корня
void m(struct Node *node)
{
    if (node == NULL)
        return;
    print_int(node);
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
// сбор списка с нечетными элементами
struct Node *f(struct Node *node, struct Node *odd)
{
    if (node == NULL)
        return odd;
    long nv = node->val;
    if (p(nv))
        odd = add_element(nv, odd);
    f(node->next, odd);
}

int main()
{
    long data[] = {4L, 8L, 15L, 16L, 23L, 42L};
    int data_length = sizeof(data) / 8;
    struct Node *node = NULL;
    for (int i = data_length - 1; i >= 0; i--)
        node = add_element(data[i], node);
    m(node);
    puts("\0");
    struct Node *odd = NULL;
    odd = f(node, odd);
    m(odd);
    puts("\0");
    // добавлено. нет в main.asm
    free_nodes(node);
    free_nodes(odd);
    return 0;
}