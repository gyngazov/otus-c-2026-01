#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <postgresql/libpq-fe.h>

#include "utils.h"

#define COPY    "COPY barcodes (id, code, val) FROM STDIN WITH (FORMAT text);"
#define PG_ROW  "%d\t%s\t%s\n"

void check_status(PGconn *conn, PGresult *res, ExecStatusType expected) {
    if (PQresultStatus(res) != expected) {
        fprintf(stderr, "Error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    PQclear(res);
}

char buf[ROW_LEN];

int insert(PGconn *conn, struct Batch batch) 
{
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


int main() {
    // 1. Establish database connection
    const char *conninfo = "dbname=testdb user=xtr password=123 host=localhost";
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    PGresult *res = PQexec(conn, COPY);
    check_status(conn, res, PGRES_COPY_IN);

    const struct SourceRow sr1 = {123, "w2e3", "u8y7"};
    const struct SourceRow sr2 = {12345, "w2kke3", "u8y7xx"};
    const struct SourceRow sr3 = {1234, "ffw2e3", "nnu8y7"};
    struct Batch b;
    b.size = 3;
    b.rows[0] = sr1;
    b.rows[1] = sr2;
    b.rows[2] = sr3;
    
    insert(conn, b);

    printf("Bulk copy completed successfully.\n");

    // Clean up
    PQfinish(conn);
    return 0;
}

void r(PGconn *conn)
{
    // 3. Prepare data buffer (Tab separated values per line)
    // Format: "id\tname\tage\n"
    const char *row1 = "1\tAlice\t30\n";
    const char *row2 = "2\tBob\t25\n";
    const char *row3 = "3\tCharlie\t35\n";

    // 4. Send rows sequentially
    if (PQputCopyData(conn, row1, strlen(row1)) != 1) {
        fprintf(stderr, "Failed to send row 1: %s\n", PQerrorMessage(conn));
    }
    if (PQputCopyData(conn, row2, strlen(row2)) != 1) {
        fprintf(stderr, "Failed to send row 2: %s\n", PQerrorMessage(conn));
    }
    if (PQputCopyData(conn, row3, strlen(row3)) != 1) {
        fprintf(stderr, "Failed to send row 3: %s\n", PQerrorMessage(conn));
    }

    // 5. Signal the end of the COPY operation
    // Pass NULL as the second argument to commit successfully
}

