#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

#include "crc32.h"

#define PAGE_SIZE 4096

uint32_t crc_block(int page, int fd, long i, uint32_t cr);

int main(int argc, char **argv) 
{
    if (argc != 2) {
        printf("usage: %s {file for crc 32}", argv[0]);
        return EXIT_FAILURE;
    }
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("cannot open file");
        return(EXIT_FAILURE);
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("cannot stat file");
        return(EXIT_FAILURE);
    }
    long sz = sb.st_size;
    uint32_t cr = ~0U;
    long i = 0;

    for (; i < sz - PAGE_SIZE; i += PAGE_SIZE)
        cr = crc_block(PAGE_SIZE, fd, i, cr);
    long tail = sz - i;
    if (tail > 0) 
        cr = crc_block(tail, fd, i, cr);
    printf("file: %s crc32: %u\n", argv[1], cr ^ ~0U);
    if (close(fd) == -1) {
        perror("cannot close file");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

uint32_t crc_block(int page, int fd, long i, uint32_t cr)
{
    void *p = mmap(NULL, page, PROT_READ, MAP_SHARED, fd, i);
    if (p == MAP_FAILED) {
        perror("cannot mmap");
        exit(EXIT_FAILURE);
    }
    cr = crc(p, page, cr);
    if (munmap(p, page) == -1) {
        perror("cannot unmap");
        exit(EXIT_FAILURE);
    }
    return cr;
}