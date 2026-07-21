
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

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
    MYSQL_BIND result_bind[3];

    // Инициализируем дескриптор
    conn = mysql_init(NULL);
    if (conn == NULL) {
        err_text = "Error: failed to create MySQL descriptor";
        goto err;
    }

    // Устанавливаем соединение с сервером
    if (!mysql_real_connect(conn, "localhost", "xtr", "123", "bark", 3306, NULL, 0)) {
        err_text = "Error: failed to connect to database:";
        goto err;
    }

    stmt = mysql_stmt_init(conn);
    if (mysql_stmt_prepare(stmt, SELECT, strlen(SELECT))) {
        fprintf(stderr, "Ошибка подготовки: %s", mysql_stmt_error(stmt));
        return 1;
    }

    const long start = 123, last = 1234;
 
    params_bind[0].buffer_type = MYSQL_TYPE_LONG;
    params_bind[0].buffer = (char *) &start;
    params_bind[1].buffer_type = MYSQL_TYPE_LONG;
    params_bind[1].buffer = (char *) &last;

    if (mysql_stmt_bind_param(stmt, bind)) {
        fprintf(stderr, "Ошибка привязки параметра: %s", mysql_stmt_error(stmt));
        return 1;
    } 

    // Выполнение запроса
    if (mysql_stmt_execute(stmt)) {
        fprintf(stderr, "Ошибка выполнения запроса: %s", mysql_stmt_error(stmt));
        return 1;
    }

    // Получение метаданных (количество столбцов)
    if (mysql_stmt_result_metadata(stmt)) {
        fprintf(stderr, "Ошибка получения метаданных
");
        return 1;
    }

    int id;
    char *code, *val;

    result_bind[0].buffer_type = MYSQL_TYPE_LONG;
    result_bind[0].buffer = (void *)&id;
    result_bind[0].buffer_length = sizeof(id);

    result_bind[1].buffer_type = MYSQL_TYPE_STRING;
    result_bind[1].buffer = (void *)code;
    result_bind[1].buffer_length = 150;

    result_bind[2].buffer_type = MYSQL_TYPE_STRING;
    result_bind[2].buffer = (void *)val;
    result_bind[2].buffer_length = 255;

    if (mysql_stmt_bind_result(stmt, result_bind)) {
        fprintf(stderr, "Ошибка привязки результата: %s", mysql_stmt_error(stmt));
        return 1;
    }

    // Извлечение и вывод данных
    while (mysql_stmt_fetch(stmt))
        printf("%d %s %d\n", id, code, val);

    mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);

    int ret = EXIT_SUCCESS;
    goto ex;

err:
    puts(err_text);
    puts(mysql_error(conn));
    ret = EXIT_FAILURE;
ex:
    mysql_close(conn);
    return ret;
}