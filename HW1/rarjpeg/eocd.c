#include <stdio.h>
#include <stdint.h>
#define EOCD_MIN_LEN 22

int main (int argc, char** argv)
{
    FILE *fp;
	fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror("Error opening jpegzip file");
        return 2;
    }
    uint32_t signature;
    fseek(fp, 0, SEEK_END);
    size_t zip_len = ftell(fp);
    for (size_t offset = zip_len - EOCD_MIN_LEN; offset != 0; offset--) {
        fseek(fp, offset, SEEK_SET);
        signature = 0;
        fread((unsigned char *) &signature, 1, 4, fp);
        if (0x06054b50 == signature) {
            printf("ura!\n");
            break;
        } else {
            printf("not found eocd\n");
        }
    }
    fclose(fp);
    return 0;
}
