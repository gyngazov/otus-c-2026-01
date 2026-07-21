#include <mysql/mysql.h>

#include "utils.h"

void view(struct Batch *b);
struct Batch *collect(MYSQL *conn, const int start, const int last);
/**
 * Воркер писатель в очередь
 * Пишет пачками размером до BATCH
 * На входе у потока:
 * - большой диапазон размером (max(id) - min(id)) / (количество потоков)
 * - дескриптор очереди
 */
void *writer (void *data);