#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

void main(int argc, char **argv)
{
    GHashTable *h1 = init();
    GHashTable *h2 = init();

    inc(h1, "sed", 3);
    inc(h2, "sed", 7);
    inc(h1, "dfg", 2);
    inc(h2, "dfg", 9);
    inc(h1, "xsd", 4);
    inc(h2, "njk", 5);
    get_top_n(h1, 3);
    get_top_n(h2, 3);
    merge(h1, h2);
    get_top_n(h1, 4);
    printf("sum: %d\n", sum(h1));
    destroy(h1);
    destroy(h2);
}

void divide(const int threads_n, const int files_n)
{
    const int base = files_n / threads_n;
    const int rest = files_n % threads_n;
    const int n = threads_n >= files_n ? files_n : threads_n;
    int distr[n];
    int i = 0;
    for (; i < rest; distr[i++] = base + 1);
    for (; i < n; distr[i++] = base);
    for (int i = 0; i < n; i++)
        printf("%d ", distr[i]);
}
