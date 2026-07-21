
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
    MYSQL *conn;        // Дескриптор соединения
    MYSQL_RES *res;      // Результат запроса
    MYSQL_ROW row;       // Строка результата
    char *err_text;
    MYSQL_STMT *stmt;
    MYSQL_BIND params_bind[2];

    // Инициализируем дескриптор
    conn = mysql_init(NULL);
    if (conn == NULL) {
        err_text = "Error: failed to create MySQL descriptor";
        goto err;
    }

    // Устанавливаем соединение с сервером
    if (!mysql_real_connect(conn, "10.0.59.96", "xtr", "123", "bark", 3306, NULL, 0)) {
        err_text = "Error: failed to connect to database:";
        goto err;
    }

    stmt = mysql_stmt_init(conn);
    if (mysql_stmt_prepare(stmt, SELECT, strlen(SELECT))) {
        fprintf(stderr, "Ошибка подготовки: %s", mysql_stmt_error(stmt));
        return 1;
    }

    unsigned long length1[2];
    bool          is_null1[2];
    bool          error1[2];
    const long start = 10, last = 122;
    memset(params_bind, 0, sizeof(params_bind));
    params_bind[0].buffer_type = MYSQL_TYPE_LONG;
    params_bind[0].buffer = (char *) &start;
    params_bind[0].is_null = &is_null1[0];
    params_bind[0].length = &length1[0];
    params_bind[0].error = &error1[0];

    params_bind[1].buffer_type = MYSQL_TYPE_LONG;
    params_bind[1].buffer = (char *) &last;
    params_bind[1].is_null = &is_null1[1];
    params_bind[1].length = &length1[1];
    params_bind[1].error = &error1[1];

    if (mysql_stmt_bind_param(stmt, params_bind)) {
        fprintf(stderr, "Ошибка привязки параметра: %s", mysql_stmt_error(stmt));
        return 1;
    } 

    // Выполнение запроса
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "Ошибка выполнения запроса: %s", mysql_stmt_error(stmt));
        return 1;
    }

    MYSQL_BIND result_bind[3];
    int           id_data;
    char          code_data[150];
    char          val_data[255];
    unsigned long length[3];
    bool          is_null[3];
    bool          error[3];

    memset(result_bind, 0, sizeof(result_bind));

    result_bind[0].buffer_type = MYSQL_TYPE_LONG;
    result_bind[0].buffer = (char *)&id_data;
    result_bind[0].is_null = &is_null[0];
    result_bind[0].length = &length[0];
    result_bind[0].error = &error[0];

/* STRING COLUMN */
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
        fprintf(stderr, "Ошибка привязки результата: %s", mysql_stmt_error(stmt));
        return 1;
    }

    // Извлечение и вывод данных
    int i = 11;
    int ret;
    while (i-- > 0) {
        ret = mysql_stmt_fetch(stmt);
        if (ret == 1 || ret == MYSQL_NO_DATA) {
            printf("ret: %d\n", ret);
            break;
        }
        printf("%d %s %s\n", id_data, code_data, val_data);
    }
        

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

    ret = EXIT_SUCCESS;
    goto ex;

err:
    puts(err_text);
    puts(mysql_error(conn));
    ret = EXIT_FAILURE;
ex:
    mysql_close(conn);
    return ret;
}