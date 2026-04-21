#include <glib-2.0/glib/ghash.h>

void test()
{
    GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);

    // Вставка пар «ключ — значение»
    g_hash_table_insert(hash, "key1", "value1");
    g_hash_table_insert(hash, "key2", "value2");

    // Получение значения по ключу
    char *value = g_hash_table_lookup(hash, "key1");

    // Удаление элемента
    g_hash_table_remove(hash, "key1");

    // Освобождение памяти
    g_hash_table_destroy(hash);

    return 0;
}