#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

char* get_content(FILE *fp);
void get_root_children(cJSON *root);
cJSON *get_field(cJSON *from, int steps);

int main (int argc, char** argv)
{
    if (argc != 2) {
        printf("usage: %s json_file_name\n", argv[0]);
        return 9;
    }
    FILE *fp = fopen(argv[1], "rb");
    char *total = get_content(fp);
    cJSON *root = cJSON_Parse(total);
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(root);
        return 8;
    }
    cJSON *field;
    field = cJSON_GetArrayItem(root->child, 0)->child; 
    field = get_field(field, 4);
    printf("%s: %s\n", field->string, cJSON_GetStringValue(field));
    field = get_field(field, 6);
    printf("%s: %s\n", field->string, cJSON_GetStringValue(field));
    cJSON_Delete(root);
    fclose(fp);
    free(total);
    return 0;

}

cJSON *get_field(cJSON *from, int steps)
{
    cJSON *field;
    for (field = from; field != NULL, steps > 0; field = field->next, steps--);
    return field;
}

void get_children(cJSON *root)
{
    cJSON *field;
    for (field = root->child; field != NULL; field = field->next)
        printf("type: %d name: %s\n", field->type, field->string);
}

char* get_content(FILE *fp)
{
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    char *total = malloc(fsize + 1);
    fseek(fp, 0, SEEK_SET);
    fread(total, fsize, 1, fp);
    total[fsize] = 0;
    return total;
}
