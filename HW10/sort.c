#include <stdio.h>
#include <stdlib.h>
struct Hit {
    char url[1024];
    int rate;
};

int compare_hits(const void* a, const void* b) {
    const struct Hit *h1 = (struct Hit *) a;
    const struct Hit *h2 = (struct Hit *) b;
    if (h1->rate > h2->rate) return -1;
    if (h1->rate < h2->rate) return 1;
    return 0;
}

int main() {
    const struct Hit hits[] = {{"swde", 17}, {"gt6y", 9}, {"gttt6y", 9}, {"vfbh", 12}, {"bgnh", 7}};
    qsort(hits, 5, sizeof(struct Hit), compare_hits);
    struct Hit hit;
    for (int i = 0; i < 5; i++) {
        hit = hits[i];
        printf("%s %d\n", hit.url, hit.rate);
    }
    return 0;
}