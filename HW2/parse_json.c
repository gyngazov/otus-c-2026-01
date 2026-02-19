#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

char* get_content(FILE *fp);
cJSON *get_field(cJSON *from, int steps);
cJSON *show_field(cJSON *from, int steps);
cJSON *get_0child(cJSON * field);
cJSON *get_root(FILE *fp);

int main (int argc, char** argv)
{
    if (argc != 2) {
        printf("usage: %s json_file_name\n", argv[0]);
        return 9;
    }
    FILE *fp = fopen(argv[1], "rb");
    cJSON *root = get_root(fp);
    cJSON *field;
    field = get_0child(root->child); 
    field = show_field(field, 4);
    field = show_field(field, 6);
    field = get_field(field, 6);
    printf("%s: %s\n", field->string, cJSON_GetStringValue(get_0child(field)));
    field = show_field(field, 3);
    field = show_field(field, 1);

    cJSON_Delete(root);
    return 0;

}

cJSON *get_root(FILE *fp)
{
    char *total = get_content(fp);
    cJSON *root = cJSON_Parse(total);
    free(total);
    if (root == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(root);
        exit(8);
    }
    return root;
}

cJSON *get_0child(cJSON * field)
{
    return cJSON_GetArrayItem(field, 0)->child;
}

cJSON *show_field(cJSON *from, int steps)
{
    cJSON *field;
    field = get_field(from, steps);
    printf("%s: %s\n", field->string, cJSON_GetStringValue(field));
    return field;
}

cJSON *get_field(cJSON *from, int steps)
{
    cJSON *field;
    for (field = from; field != NULL && steps > 0; steps--)
        field = field->next;
    return field;
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
