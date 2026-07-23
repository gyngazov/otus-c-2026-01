#include <stdio.h>
#include <time.h>

int main() {
    struct timespec start, end;

    // Замеряем начало
    clock_gettime(CLOCK_REALTIME, &start);

    // Здесь — код, время работы которого измеряем
    // ...

    // Замер окончания
    clock_gettime(CLOCK_REALTIME, &end);

    // Вычисляем разницу в наносекундах
    long long elapsed_ns = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);

    printf("Время работы: %lld наносекунд", elapsed_ns);
    return 0;
}