#include <glib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // 1. Initialize an empty list
    GList* list = NULL;

    // 2. Add items to the list
    list = g_list_append(list, "First Item");
    list = g_list_append(list, "Second Item");
    
    // 3. Prepend an item (adds to the front)
    list = g_list_prepend(list, "Zero Item");

    // 4. Iterate and print all items
    printf("--- List Contents ---\n");
    GList* l;
    for (l = list; l != NULL; l = l->next) {
        printf("%s\n", (char*)l->data);
    }

    // 5. Cleanup memory
    g_list_free(list);

    return 0;
}
