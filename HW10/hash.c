#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// нужен одинаковый размер для qsort
#define URL_LEN 4096
#define EHASHTAB -11
#define PRCO printf("%d\n", __COUNTER__);

static gpointer *ht_keys;
static int ht_size;

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
    GHashTable *ght = g_hash_table_new(g_str_hash, g_str_equal);
    if (ght == NULL) {
        printf("Не создалась хеш-таблица.\n");
        exit(EHASHTAB);
    }
    return ght;
}

void set_keys(GHashTable *hash)
{
    ht_keys = g_hash_table_get_keys_as_array(hash, NULL);
    ht_size = g_hash_table_size(hash);
}

void destroy(GHashTable *hash)
{
    set_keys(hash);
    for (int i = 0; i < ht_size; free(ht_keys[i++]));
    g_hash_table_destroy(hash);
}

// увеличить по ключу
// исключать +0 или ключ = "-"
void inc(GHashTable *hash, char *key, const int plus)
{
    if (hash == NULL || plus <= 0 || strlen(key) == 1)
        return;
    PRCO
    gpointer value = g_hash_table_lookup(hash, key);
    PRCO
    char *ins = g_strdup(key);
    PRCO
    if (ins == NULL) {
        perror("Не выделена память");
        exit(errno);
    }
    int new = plus;
    if (value != NULL)
        new += GPOINTER_TO_INT(value);
    g_hash_table_insert(hash, ins, GINT_TO_POINTER(new));
}

// топ n наибольших значений у hash
// с ключами
void get_top_n(GHashTable *hash, const int n)
{
    if (is_empty(hash) || n <= 0)
        return;
    set_keys(hash);
    gpointer value;
    char *key;
    struct Hit hits[ht_size];
    for (int i = 0; i < ht_size; i++) {
        key = (char *) ht_keys[i];
        value = g_hash_table_lookup(hash, key);
        memcpy(hits[i].url, key, strlen(key) + 1);
        hits[i].rate = GPOINTER_TO_INT(value);
    }
    free(ht_keys);
    qsort(hits, ht_size, sizeof(struct Hit), compare_hits);
    const int min = ht_size > n ? n : ht_size;
    for (int i = 0; i < min; i++)
        printf("%d: %s -> %d\n", i + 1, hits[i].url, hits[i].rate);
}

// добавить хт new в хт hash
void merge(GHashTable *hash, GHashTable *new)
{
    if (is_empty(hash) && is_empty(new))
        return;
    set_keys(new);
    gpointer key, v1;
    for (int i = 0; i < ht_size; i++) {
        key = ht_keys[i];
        v1 = g_hash_table_lookup(new, key);
        inc(hash, key, GPOINTER_TO_INT(v1));
    }
    free(ht_keys);
}

// сумма всех значений
int sum(GHashTable *hash)
{
    if (is_empty(hash))
        return 0;
    set_keys(hash);
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
    set_keys(hash);
    gpointer val;
    printf("size: %d\n", ht_size);
    for (int i = 0; i < ht_size; i++) {
        val = g_hash_table_lookup(hash, ht_keys[i]);
        printf("%s -> %d\n", (char *) ht_keys[i], GPOINTER_TO_INT(val));
    }
    free(ht_keys);
}

