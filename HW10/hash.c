#include <glib.h>
#include <stdio.h>

void main()
{
    GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);

    // Вставка пар «ключ — значение»
    gint *val;
    *val = 7;
    g_hash_table_insert(hash, "key1", val);
    //g_hash_table_insert(hash, "key2", 17);

    // Получение значения по ключу
    gpointer value = g_hash_table_lookup(hash, "key1");
    int vval = GPOINTER_TO_INT(value);
    printf("val: %d\n", vval);
    // Удаление элемента
    g_hash_table_remove(hash, "key1");

    // Освобождение памяти
    g_hash_table_destroy(hash);

}