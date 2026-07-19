
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    MYSQL *conn;        // Дескриптор соединения
    MYSQL_RES *res;      // Результат запроса
    MYSQL_ROW row;       // Строка результата
    char *err_text;

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

    // Выполняем SQL-запрос
    if (mysql_query(conn, "SELECT * FROM barcodes")) {
        err_text = "Error: query failed:";
        goto err;
    }

    // Получаем результат запроса
    res = mysql_store_result(conn);
    if (res == NULL) {
        err_text = "Error: failed to store result:";
        goto err;
    }

    while ((row = mysql_fetch_row(res)) != NULL) {
        printf("%s ", row[0]);
        printf("%s ", row[1]);
        printf("%s ", row[2]);
    }

    mysql_free_result(res);
    int ret = EXIT_SUCCESS;
    goto ex;

err:
    printf(err_text);
    printf("%s\n", mysql_error(conn));
    ret = EXIT_FAILURE;
ex:
    mysql_close(conn);
    return ret;
}