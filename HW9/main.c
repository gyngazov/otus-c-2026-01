#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include "crc32.h"

#define BLOCK 256

uint32_t crc_round(const void *p, long size, uint32_t crc);

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s {filename}\n", argv[0]);
        return -1;
    }
    struct stat stt;
    stat(argv[1], &stt);
    long file_size = stt.st_size;
    printf("size: %ld\n", file_size);

    int shm_fd = shm_open(argv[1], O_RDONLY | O_CREAT, 0664);
    if (shm_fd == -1) {
        perror("shm_open");
        printf("%d\n", shm_fd);
        return 1;
    }
    printf("file opened\n");
    void *p;
    long i = 0, tail;
    uint32_t crc = ~0U;

    for (; i < file_size - BLOCK; i += BLOCK) {
        p = mmap(0, BLOCK, PROT_READ, MAP_SHARED, shm_fd, i);
        if (p == MAP_FAILED) {
            perror("mmap");
            close(shm_fd);
            return 1;
        }
        printf("mmaped %ld\n", i);
        crc = crc_round(p, BLOCK, crc);
    }
    tail = file_size - i;
    if (tail > 0) {
        p = mmap(0, tail, PROT_READ, MAP_SHARED, shm_fd, i);
        if (p == MAP_FAILED) {
            perror("mmap");
            close(shm_fd);
            return 1;
        }
        crc = crc_round(p, tail, crc);
    }

    if (munmap(p, file_size) == -1) {
        perror("munmap");
    }
    close(shm_fd);
    printf("crc32 = %d\n", crc ^ ~0U);

    return EXIT_SUCCESS;
}

uint32_t crc_round(const void *p, long size, uint32_t tmp)
{
    uint32_t crc = tmp;
    const uint8_t *p8 = p;
    for (long j = 0; j < size; j++) {
        printf("crc %ld: %ld\n", j, crc);
        printf("p: %d\n", *p8);
        printf("i=%d\n", (crc ^ *p8) & 0xFF);
        crc = crc32_tab[(crc ^ *p8++) & 0xFF] ^ (crc >> 8);
    }
    return crc;
}

