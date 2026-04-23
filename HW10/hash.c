#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// нужен одинаковый размер для qsort
#define URL_LEN 1024

struct Hit {
    char url[URL_LEN];
    int rate;       // частота или суммарный трафик по урлу
};

static int is_empty(GHashTable *hash)
{
    return (hash == NULL || g_hash_table_size(hash) == 0) ? 1 : 0;
}

// компаратор по убыванию
static int compare_hits(const void* a, const void* b) {
    const struct Hit *h1 = (struct Hit *) a;
    const struct Hit *h2 = (struct Hit *) b;
    if (h1->rate > h2->rate) 
        return -1;
    if (h1->rate < h2->rate) 
        return 1;
    return 0;
}

GHashTable *init()
{
    return g_hash_table_new(g_str_hash, g_str_equal);
}

void destroy(GHashTable *hash)
{
    g_hash_table_destroy(hash);
}

// увеличить по ключу
// исключать +0 или ключ = "-"
void inc(GHashTable *hash, char *key, const int plus)
{
    if (hash == NULL || plus <= 0 || strlen(key) == 1)
        return;
    gpointer value = g_hash_table_lookup(hash, key);
    char *ins = g_strdup(key);
    int new = plus;
    if (value != NULL) {
        new += GPOINTER_TO_INT(value);
    }
    printf("key: %s\n", key);
    g_hash_table_insert(hash, ins, GINT_TO_POINTER(new));
}

// топ n наибольших значений у hash
// с ключами
void get_top_n(GHashTable *hash, const int n)
{
    if (is_empty(hash) || n <= 0)
        return;
    gpointer *keys = g_hash_table_get_keys_as_array(hash, NULL);
    const int size = g_hash_table_size(hash);
    gpointer value;
    char *key;
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

// добавить хт new в хт hash
void merge(GHashTable *hash, GHashTable *new)
{
    if (is_empty(hash) && is_empty(new))
        return;
    gpointer *keys = g_hash_table_get_keys_as_array(new, NULL);
    const int size = g_hash_table_size(new);
    gpointer key, v1;
    for (int i = 0; i < size; i++) {
        key = keys[i];
        v1 = g_hash_table_lookup(new, key);
        inc(hash, key, GPOINTER_TO_INT(v1));
    }
    free(keys);
}

// сумма всех значений
int sum(GHashTable *hash)
{
    if (is_empty(hash))
        return 0;
    gpointer *keys = g_hash_table_get_keys_as_array(hash, NULL);
    const int size = g_hash_table_size(hash);
    gpointer val;
    int sum = 0;
    for (int i = 0; i < size; i++) {
        val = g_hash_table_lookup(hash, keys[i]);
        sum += GPOINTER_TO_INT(val);
    }
    free(keys);
    return sum;
}

void view(GHashTable *hash)
{
    if (is_empty(hash))
        return;
    gpointer *keys = g_hash_table_get_keys_as_array(hash, NULL);
    const int size = g_hash_table_size(hash);
    gpointer val;
    printf("size: %d\n", size);
    for (int i = 0; i < size; i++) {
        val = g_hash_table_lookup(hash, keys[i]);
        printf("%s -> %d\n", (char *) keys[i], GPOINTER_TO_INT(val));
    }
    free(keys);
}

