#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>

void check_status(PGconn *conn, PGresult *res, ExecStatusType expected) {
    if (PQresultStatus(res) != expected) {
        fprintf(stderr, "Error: %s\n", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }
    PQclear(res);
}

int main() {
    // 1. Establish database connection
    const char *conninfo = "dbname=testdb user=postgres password=secret host=localhost";
    PGconn *conn = PQconnectdb(conninfo);

    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection failed: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    // 2. Start the COPY IN command
    // This example populates a table named 'users' with columns 'id', 'name', and 'age'
    PGresult *res = PQexec(conn, "COPY users (id, name, age) FROM STDIN WITH (FORMAT text);");
    check_status(conn, res, PGRES_COPY_IN);

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
    if (PQputCopyEnd(conn, NULL) != 1) {
        fprintf(stderr, "Failed to terminate COPY: %s\n", PQerrorMessage(conn));
    }

    // 6. Mandatory step: Fetch the command execution result
    res = PQgetResult(conn);
    check_status(conn, res, PGRES_COMMAND_OK);

    printf("Bulk copy completed successfully.\n");

    // Clean up
    PQfinish(conn);
    return 0;
}

