#include <stdio.h>

int main(int argc, char** argv)
{
    FILE *fp;
	fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror("Error opening output file");
        return 2;
    }
    int b1, b2;
    if ((b1 = fgetc(fp)) != EOF && b1 == 255 
     && (b2 = fgetc(fp)) != EOF && b2 == 216)
        printf("%d-%x %d-%x\n", b1, b1, b2, b2);
    fseek(fp, -2L, SEEK_END);
    if ((b1 = fgetc(fp)) != EOF //&& b1 == 255 
     && (b2 = fgetc(fp)) != EOF /*&& b2 == 216*/)
        printf("%d-%x %d-%x\n", b1, b1, b2, b2);
    return 0;
}
