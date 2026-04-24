#include <glib.h>

GHashTable *init();

void destroy(GHashTable *hash);

void inc(GHashTable *hash, const char *key, const int plus);

void get_top(GHashTable *hash);

void merge(GHashTable *hash, GHashTable *new);

int sum(GHashTable *hash);

void view(GHashTable *hash);

// http://grep.cs.msu.ru/Manage_C_data_using_the_GLib.html