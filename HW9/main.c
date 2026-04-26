#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

#include "crc32.h"

#define PAGE_SIZE 4096

int crc_block(int page, int fd, long i, uint32_t cr);
int getopt(int argc, char **argv, char *opts);

int main(int argc, char **argv) 
{
    int opt;
    extern char *optarg;
    char *file = NULL;
    while((opt = getopt(argc, argv, ":f:")) != -1) { 
        switch(opt) {  
            case 'f': 
                file = optarg;
                break;
            case ':': 
                printf("Не указано имя файла\n"); 
                exit(EXIT_FAILURE); 
            case 'h':
            case '?': 
            default:
                printf("usage: %s -f {file_to_crc}\n", argv[0]); 
                exit(EXIT_FAILURE);
        } 
    }
    if (file == NULL) {
        printf("usage: %s -f {file_to_crc}\n", argv[0]); 
        exit(EXIT_FAILURE);
    }
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        perror("cannot open file");
        exit(EXIT_FAILURE);
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("cannot stat file");
        exit(EXIT_FAILURE);
    }
    long sz = sb.st_size;
    long i = 0;
    int res = ~0U;

    for (; i < sz - PAGE_SIZE; i += PAGE_SIZE) {
        res = crc_block(PAGE_SIZE, fd, i, (uint32_t) res);
        if (res == -1) {
            printf("Ошибка памяти\n");
            exit(EXIT_FAILURE);
        }
    }
    long tail = sz - i;
    if (tail > 0) 
        res = crc_block(tail, fd, i, (uint32_t) res);
    if (res == -1) {
        printf("Ошибка памяти\n");
        exit(EXIT_FAILURE);
    }
    printf("file: %s crc32: %u\n", argv[1], res ^ ~0U);
    if (close(fd) == -1) {
        perror("cannot close file");
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

int crc_block(int page, int fd, long i, uint32_t cr)
{
    void *p = mmap(NULL, page, PROT_READ, MAP_SHARED, fd, i);
    if (p == MAP_FAILED) {
        perror("cannot mmap");
        return -1;
    }
    cr = crc(p, page, cr);
    if (munmap(p, page) == -1) {
        perror("cannot unmap");
        return -1;
    }
    return cr;
}