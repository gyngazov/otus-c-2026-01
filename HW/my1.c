
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define SELECT "select id, code, val from barcodes where id between ? and ?"

struct Range {
    int start;
    int last;
};

int main() {

    MYSQL *conn;        
    char *err_text = "";
    int ret = EXIT_FAILURE;

    conn = mysql_init(NULL);
    if (conn == NULL) {
        err_text = "НЕ создан дескриптор mysql";
        goto err;
    }

    if (!mysql_real_connect(conn, "10.0.59.96", "xtr", "123", "bark", 3306, NULL, 0)) {
        err_text = "Ошибка подключения к бд";
        goto err;
    }

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
    const long start = 10, last = 122;

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

    while (1) {
        rc = mysql_stmt_fetch(stmt);
        if (rc == 1 || rc == MYSQL_NO_DATA)
            break;
        printf("%d %s %s\n", id_data, code_data, val_data);
    }

    ret = EXIT_SUCCESS;
sterr:
    puts(mysql_stmt_error(stmt));
    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

err:
    puts(err_text);
    puts(mysql_error(conn));
ex:
    if (conn != NULL)
        mysql_close(conn);
    return ret;
}