#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

#include "utils.h"
#include "thsque.h"

#define COPY    "COPY barcodes (id, code, val) FROM STDIN WITH (FORMAT text);"
#define PG_ROW  "%d\t%s\t%s\n"
#define PG_CONN "dbname=testdb user=root password=123 host=localhost"

static void check_status(PGconn *conn, PGresult *res, ExecStatusType expected) {
    if (PQresultStatus(res) != expected) {
        fprintf(stderr, "Error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    PQclear(res);
}

static int insertp(PGconn *conn, struct Batch *batch) 
{
    char buf[ROW_LEN];
    struct SourceRow sr;
    PGresult *res = PQexec(conn, COPY);
    check_status(conn, res, PGRES_COPY_IN);
    for (int i = 0; i < batch->size; i++) {
        sr = batch->rows[i];
        if (snprintf(buf, ROW_LEN, PG_ROW, sr.id, sr.code, sr.val) < 0) {
            puts("Ошибка копирования");
            return -1;
        }
        if (PQputCopyData(conn, buf, strlen(buf)) != 1) {
            fprintf(stderr, "Failed to send row: %s error: %s\n", buf, PQerrorMessage(conn));
            return -1;
        }
    }
    if (PQputCopyEnd(conn, NULL) != 1) {
        fprintf(stderr, "Failed to terminate COPY: %s\n", PQerrorMessage(conn));
        return -1;
    }

    res = PQgetResult(conn);
    free(batch);
    check_status(conn, res, PGRES_COMMAND_OK);
    return 0;
}

// чтение очереди
void *reader(void *data)
{
    struct ThreadData *thd = (struct ThreadData *) data;
    PGconn *conn = PQconnectdb(PG_CONN);
    if (PQstatus(conn) != CONNECTION_OK) {
        printf("Connection failed: %s\n", PQerrorMessage(conn));
        goto ex;
    }
    
    void *b;
    while((b = queue_pop(thd->tsq)) != NULL)
        if (insertp(conn, (struct Batch *) b) == -1)
            break;

ex:
    PQfinish(conn);
    free(thd);
    return NULL;
}

