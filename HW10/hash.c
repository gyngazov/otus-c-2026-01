#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// нужен одинаковый размер для qsort
#define URL_LEN     8192
#define EHASHTAB    -11
#define TOP         10

struct Hit {
    char url[URL_LEN];
    int rate;       // частота по рефереру или суммарный трафик по пути урла
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

// освободит вытесняемый ключ при g_hash_table_insert
// для init
static void free_key(gpointer data) 
{
    free(data);
}

GHashTable *init()
{
    GHashTable *ght = g_hash_table_new_full(g_str_hash, g_str_equal, (GDestroyNotify)free_key, NULL);
    if (ght == NULL) {
        printf("Не создалась хеш-таблица.\n");
        exit(EHASHTAB);
    }
    return ght;
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
    if (ins == NULL) {
        perror("Не выделена память");
        exit(errno);
    }
    int new = plus;
    if (value != NULL)
        new += GPOINTER_TO_INT(value);
    g_hash_table_insert(hash, ins, GINT_TO_POINTER(new));
}

// TOP наибольших значений у hash
// с ключами
void get_top(GHashTable *hash)
{
    gpointer value;
    char *key;
    gpointer *ht_keys = g_hash_table_get_keys_as_array(hash, NULL);
    const int ht_size = g_hash_table_size(hash);
    struct Hit *hits = (struct Hit *) malloc(ht_size * sizeof(struct Hit));
    for (int i = 0; i < ht_size; i++) {
        key = (char *) ht_keys[i];
        value = g_hash_table_lookup(hash, key);
        memcpy(hits[i].url, key, strlen(key) + 1);
        hits[i].rate = GPOINTER_TO_INT(value);
    }
    free(ht_keys);
    qsort(hits, ht_size, sizeof(struct Hit), compare_hits);
    const int min = ht_size > TOP ? TOP : ht_size;
    for (int i = 0; i < min; i++)
        printf("%d: %s -> %d\n", i + 1, hits[i].url, hits[i].rate);
    free(hits);
}

// добавить хт new в хт hash
void merge1(GHashTable *hash, GHashTable *new)
{
    gpointer *ht_keys = g_hash_table_get_keys_as_array(hash, NULL);
    const int ht_size = g_hash_table_size(hash);
    gpointer k, v;
    for (int i = 0; i < ht_size; i++) {
        k = ht_keys[i];
        v = g_hash_table_lookup(new, k);
        inc(hash, k, GPOINTER_TO_INT(v));
    }
    free(ht_keys);
}

void merge(GHashTable *hash, GHashTable *new)
{
    gpointer *new_keys = g_hash_table_get_keys_as_array(new, NULL);
    const int new_size = g_hash_table_size(new);
    gpointer k, v;
    for (int i = 0; i < new_size; i++) {
        k = new_keys[i];
        v = g_hash_table_lookup(new, k);
        inc(hash, k, GPOINTER_TO_INT(v));
    }
    free(new_keys);
}

// сумма всех значений
int sum(GHashTable *hash)
{
    if (is_empty(hash))
        return 0;
    gpointer *ht_keys = g_hash_table_get_keys_as_array(hash, NULL);
    const int ht_size = g_hash_table_size(hash);
    gpointer val;
    int sum = 0;
    for (int i = 0; i < ht_size; i++) {
        val = g_hash_table_lookup(hash, ht_keys[i]);
        sum += GPOINTER_TO_INT(val);
    }
    free(ht_keys);
    return sum;
}

void view(GHashTable *hash)
{
    if (is_empty(hash))
        return;
    gpointer *ht_keys = g_hash_table_get_keys_as_array(hash, NULL);
    const int ht_size = g_hash_table_size(hash);
    gpointer k, v;
    printf("size: %d\n", ht_size);
    for (int i = 0; i < ht_size; i++) {
        k = ht_keys[i];
        v = g_hash_table_lookup(hash, k);
        printf("%s -> %d\n", (char *) k, GPOINTER_TO_INT(v));
    }
    free(ht_keys);
}

