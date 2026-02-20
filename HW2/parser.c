#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "parser.h"

static cJSON *get_root(char *total);
static cJSON *get_0child(cJSON * field);
static cJSON *show_field(cJSON *from, int steps);
static cJSON *get_field(cJSON *from, int steps);

void parse_json(char *content)
{
    cJSON *root = get_root(content);
    cJSON *field;
    field = get_0child(root->child); 
    field = show_field(field, 4);
    field = show_field(field, 6);
    field = get_field(field, 6);
    printf("%s: %s\n", field->string, cJSON_GetStringValue(get_0child(field)));
    field = show_field(field, 3);
    field = show_field(field, 1);

    cJSON_Delete(root);
}

static cJSON *get_root(char *total)
{
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

static cJSON *get_0child(cJSON * field)
{
    return cJSON_GetArrayItem(field, 0)->child;
}

static cJSON *show_field(cJSON *from, int steps)
{
    cJSON *field;
    field = get_field(from, steps);
    printf("%s: %s\n", field->string, cJSON_GetStringValue(field));
    return field;
}

static cJSON *get_field(cJSON *from, int steps)
{
    cJSON *field;
    for (field = from; field != NULL && steps > 0; steps--)
        field = field->next;
    return field;
}
