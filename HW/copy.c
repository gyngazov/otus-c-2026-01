#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

#include "utils.h"

#define COPY    "COPY barcodes (id, code, val) FROM STDIN WITH (FORMAT text);"
#define SELECT  "SELECT id, code, val FROM barcodes WHERE id BETWEEN $1 AND $2"
#define PG_ROW  "%d\t%s\t%s\n"

static void check_status(PGconn *conn, PGresult *res, ExecStatusType expected) {
    if (PQresultStatus(res) != expected) {
        fprintf(stderr, "Error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    PQclear(res);
}


int insert(PGconn *conn, struct Batch batch) 
{
    char buf[ROW_LEN];
    struct SourceRow sr;
    for (int i = 0; i < batch.size; i++) {
        sr = batch.rows[i];
        snprintf(buf, ROW_LEN, PG_ROW, sr.id, sr.code, sr.val);
        if (PQputCopyData(conn, buf, strlen(buf)) != 1) {
            fprintf(stderr, "Failed to send row: %s error: %s\n", buf, PQerrorMessage(conn));
            return -1;
        }
    }
    if (PQputCopyEnd(conn, NULL) != 1) {
        fprintf(stderr, "Failed to terminate COPY: %s\n", PQerrorMessage(conn));
        return -1;
    }

    PGresult *res = PQgetResult(conn);
    check_status(conn, res, PGRES_COMMAND_OK);
    return 0;
}

int insertp(PGconn *conn, struct Batch *batch) 
{
    char buf[ROW_LEN];
    struct SourceRow sr;
    PGresult *res = PQexec(conn, COPY);
    check_status(conn, res, PGRES_COPY_IN);
    for (int i = 0; i < batch->size; i++) {
        sr = batch->rows[i];
        snprintf(buf, ROW_LEN, PG_ROW, sr.id, sr.code, sr.val);
        if (PQputCopyData(conn, buf, strlen(buf)) != 1) {
            fprintf(stderr, "Failed to send row: %s error: %s\n", buf, PQerrorMessage(conn));
            return -1;
        }
    }
    free(batch);
    if (PQputCopyEnd(conn, NULL) != 1) {
        fprintf(stderr, "Failed to terminate COPY: %s\n", PQerrorMessage(conn));
        return -1;
    }

    res = PQgetResult(conn);
    check_status(conn, res, PGRES_COMMAND_OK);
    return 0;
}

PGconn *get_conn()
{
    const char *conninfo = "dbname=testdb user=xtr password=123 host=localhost";
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return NULL;
    }
    return conn;
}

