#include <stdio.h>
#include <unistd.h> // For sleep()

int main() {
    int ar[] = {1, 7, 8, 2, 3, 4, 1, 2, 1};
    int len = sizeof(ar)/sizeof(int);
    for (int i = 0; i < len; i++) {
        printf("\rProgress: %d%%", ar[i]);
        fflush(stdout);
        usleep(1000000);
    }
    puts("");
    return 0;
}
