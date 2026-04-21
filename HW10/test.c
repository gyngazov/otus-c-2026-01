#include <stdio.h>
#include <stdlib.h>

void main(int argc, char **argv)
{
    if (argc != 3) {
        printf("no 2 args\n");
        return;
    }
    const int threads_n = atoi(argv[1]);
    const int files_n = atoi(argv[2]);
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
