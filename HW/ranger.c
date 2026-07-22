
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "thsque.h"

#define BATCH 123
#define SELECT "select id, code, val from barcodes where id between ? and ?"

static struct Batch *collect(MYSQL *conn, const int start, const int last)
{
    char *err_text = "";
    MYSQL_STMT *stmt;
    stmt = mysql_stmt_init(conn);
    if (stmt == NULL) {
        err_text = "НЕ создан дескриптор запроса";
        goto err;
    }
    if (mysql_stmt_prepare(stmt, SELECT, strlen(SELECT))) {
        err_text = "Ошибка подготовки запроса";
        goto sterr;
    }

    MYSQL_BIND params_bind[2];
    unsigned long lengthp[2];
    bool is_nullp[2];
    bool errorp[2];

    memset(params_bind, 0, sizeof(params_bind));
    params_bind[0].buffer_type = MYSQL_TYPE_LONG;
    params_bind[0].buffer = (char *) &start;
    params_bind[0].is_null = &is_nullp[0];
    params_bind[0].length = &lengthp[0];
    params_bind[0].error = &errorp[0];

    params_bind[1].buffer_type = MYSQL_TYPE_LONG;
    params_bind[1].buffer = (char *) &last;
    params_bind[1].is_null = &is_nullp[1];
    params_bind[1].length = &lengthp[1];
    params_bind[1].error = &errorp[1];

    if (mysql_stmt_bind_param(stmt, params_bind)) {
        err_text = "Ошибка привязки параметра";
        goto sterr;
    } 

    if (mysql_stmt_execute(stmt)) {
        err_text = "Ошибка выполнения запроса";
        goto sterr;
    }

    MYSQL_BIND result_bind[3];
    int id_data;
    char code_data[150];
    char val_data[255];
    unsigned long length[3];
    bool is_null[3];
    bool error[3];

    memset(result_bind, 0, sizeof(result_bind));

    result_bind[0].buffer_type = MYSQL_TYPE_LONG;
    result_bind[0].buffer = (char *)&id_data;
    result_bind[0].is_null = &is_null[0];
    result_bind[0].length = &length[0];
    result_bind[0].error = &error[0];

    result_bind[1].buffer_type = MYSQL_TYPE_STRING;
    result_bind[1].buffer = (char *)code_data;
    result_bind[1].buffer_length = 150;
    result_bind[1].is_null = &is_null[1];
    result_bind[1].length = &length[1];
    result_bind[1].error = &error[1];

    result_bind[2].buffer_type = MYSQL_TYPE_STRING;
    result_bind[2].buffer = (char *)val_data;
    result_bind[2].buffer_length = 255;
    result_bind[2].is_null = &is_null[2];
    result_bind[2].length = &length[2];
    result_bind[2].error = &error[2];

    if (mysql_stmt_bind_result(stmt, result_bind)) {
        err_text = "Ошибка привязки результата";
        goto sterr;
    }

    int rc;
    struct Batch *b;
    b = (struct Batch *) malloc(sizeof(struct Batch));
    if (b == NULL)
        return NULL;
    struct SourceRow *sr;
    int i = 0;

    while (1) {
        rc = mysql_stmt_fetch(stmt);
        if (rc == 1 || rc == MYSQL_NO_DATA)
            break;
        sr = (struct SourceRow *)malloc(sizeof(struct SourceRow));
        if (sr == NULL)
            return NULL;
        sr->id = id_data;
        snprintf(sr->code, 150, "%s", code_data);
        snprintf(sr->val, 255, "%s", val_data);
        b->rows[i] = *sr;
        i++;
    }
    b->size = i;

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);
    return b;
sterr:
    puts(mysql_stmt_error(stmt));
    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

err:
    puts(err_text);
    puts(mysql_error(conn));
    return NULL;
}
/**
 * Потоку писателю в очередь выдается диапазон id строк.
 */
void *writer(void *data) 
{
    struct ThreadData *thd = (struct ThreadData *) data;
    MYSQL *conn;        
    char *err_text = "";

    conn = mysql_init(NULL);
    if (conn == NULL) {
        err_text = "НЕ создан дескриптор mysql";
        goto nul;
    }

    if (!mysql_real_connect(conn, "10.0.59.96", "xtr", "123", "bark", 3306, NULL, 0)) {
        err_text = "Ошибка подключения к бд";
        goto err;
    }

    int begin = thd->start;
    const int end = thd->last;
    struct Batch *b;
    while (begin <= end - BATCH) {
        b = collect(conn, begin, begin + BATCH - 1);
        if (b == NULL)
            return NULL;
        begin += BATCH;
        queue_push(thd->tsq, (void *) b);
    }
    if (begin < end) {
        b = collect(conn, begin, end);
        if (b == NULL)
            return NULL;
        queue_push(thd->tsq, (void *) b);
    }
    
err:
    puts(mysql_error(conn));
    mysql_close(conn);
nul:
    puts(err_text);
    free(thd);
    return NULL;
}