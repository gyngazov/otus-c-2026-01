#include <stdio.h>

#include <libpq-fe.h>

#define PG_CONN "dbname=testdb user=root password=123 host=10.0.59.96"

int main()
{
    PGconn *conn = PQconnectdb(PG_CONN);
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Ошибка подключения: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }
    PGresult *res = PQexec(conn, "select id, code, val from barcodes limit 11");
    printf("%d\n", PQresultStatus(res));

    int cols = PQnfields(res);
    for (int i = 0; i < cols; ++i)
        printf("%s\t", PQfname(res, i));
    
    puts("");

    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j)
            printf("%s\t", PQgetvalue(res, i, j));

        puts("");
    }
    
    return 0;
}