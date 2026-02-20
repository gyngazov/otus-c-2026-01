#include <stdio.h>

#include "parser.h"
#include "weather.h"

char* get_content(FILE *fp);
int main(int argc, char** argv)
{
    char *url = "https://www.example.com";
    char *res = get_weather(url);
    printf("%s\n", res);
//    "https://wttr.in/moscow?format=j1";
    if (argc != 2) {
        printf("usage: %s input_file\n", argv[0]);
        return 8;
    }
    FILE *fp;
	fp = fopen(argv[1], "rb");
    if (fp == NULL) {
        perror("Ошибка открытия файла");
        return 7;
    }
    char *json = get_content(fp);
    parse_json(json);
    return 0;
}

char* get_content(FILE *fp)
{
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    char *total = malloc(fsize + 1);
    fseek(fp, 0, SEEK_SET);
    fread(total, fsize, 1, fp);
    fclose(fp);
    total[fsize] = 0;
    return total;
}

