#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static GHashTable *hash;

struct Hit {
    char url[1024]; // нужен одинаковый размер для qsort
    int rate;
};

static int compare_hits(const void* a, const void* b) {
    const struct Hit *h1 = (struct Hit *) a;
    const struct Hit *h2 = (struct Hit *) b;
    if (h1->rate > h2->rate) 
        return 1;
    if (h1->rate < h2->rate) 
        return -1;
    return 0;
}

void init()
{
    hash = g_hash_table_new(g_str_hash, g_str_equal);
}

void destroy()
{
    g_hash_table_destroy(hash);
}

void inc(char *key)
{
    gpointer value = g_hash_table_lookup(hash, key);
    if (value == NULL) {
        g_hash_table_insert(hash, key, GINT_TO_POINTER(1));
    } else {
        int new = GPOINTER_TO_INT(value) + 1;
        g_hash_table_replace(hash, key, GINT_TO_POINTER(new));
    }
}

void get_top_n(int n)
{
    gpointer *keys = g_hash_table_get_keys_as_array(hash, NULL);
    gpointer value;
    char *key;
    const int size = g_hash_table_size(hash);
    struct Hit hits[size];
    for (int i = 0; i < size; i++) {
        key = (char *) keys[i];
        value = g_hash_table_lookup(hash, key);
        memcpy(hits[i].url, key, strlen(key) + 1);
        hits[i].rate = GPOINTER_TO_INT(value);
    }
    free(keys);
    qsort(hits, size, sizeof(struct Hit), compare_hits);
    const int min = size > n ? n : size;
    for (int i = 0; i < min; i++)
        printf("%d: %s -> %d\n", i + 1, hits[i].url, hits[i].rate);
}

void test()
{
    init();
    inc("sdf");
    inc("vfg");
    inc("sdf");
    inc("sdf");
    inc("sdc");
    inc("vfg");
    inc("sdf");
    inc("sdc");
    inc("sdc");
    get_top_n(4);
    destroy();
}