#include <glib.h>

GHashTable * init();
void destroy(GHashTable *hash);
void inc(GHashTable *hash, const char *key, const int plus);
void get_top_n(GHashTable *hash, const int n);
void merge(GHashTable *hash, GHashTable *new);
int sum(const GHashTable *hash);
void view(GHashTable *hash);