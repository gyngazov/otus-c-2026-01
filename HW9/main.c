#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "crc32.h"

#define BLOCK 4096

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("usage: %s {filename}\n", argv[0]);
        return -1;
    }
    FILE *fd;
    fd = fopen(argv[1], "r");
    fseek(fd, 0L, SEEK_END);
    long size = ftell(fd);
    close(fd);
    //char *str = "test";
    //printf("%s crc32=%ld\n", str, crc32(str, sizeof(str)));

    int shm_fd = shm_open(argv[1], O_RDONLY, 0600);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }
    void *ptr;
    long i;
    uint32_t crc = ~0U;
    for (i = 0; i < size; i += BLOCK) {
        ptr = mmap(0, BLOCK, PROT_READ, MAP_SHARED, shm_fd, 0);
        
    if (ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    // read data
    printf("Consumer read: '%s'\n", (char *)ptr);

    // unmap from meory
    if (munmap(ptr, size) == -1) {
        perror("munmap");
    }

    // close descriptor
    close(fd);

    return 0;

    return EXIT_SUCCESS;
}