#include <stdio.h>
#include <stdlib.h>
#include <sql.h>      /* ODBC API */
#include <sqlext.h>   /* Дополнительные функции ODBC */
#include <string.h>

#define PERSONID_LEN 2
#define LASTNAME_LEN 256
#define FIRSTNAME_LEN 256
#define ADDRESS_LEN 256
#define CITY_LEN 256

int main() {
    SQLHENV henv = SQL_NULL_HENV;        /* Дескриптор среды ODBC */
    SQLHDBC hdbc = SQL_NULL_HDBC;        /* Дескриптор подключения */
    SQLHSTMT hstmt = SQL_NULL_HSTMT;      /* Дескриптор оператора */
    SQLRETURN retcode;

    /* Буферы для хранения данных из результирующего набора */
    SQLCHAR strFirstName[FIRSTNAME_LEN], strLastName[LASTNAME_LEN];
    SQLCHAR strAddress[ADDRESS_LEN], strCity[CITY_LEN];

    /* Длины буферов */
    SQLLEN lenFirstName = 0, lenLastName = 0, lenAddress = 0, lenCity = 0;

    /* Переменная для ID персоны */
    SQLINTEGER cPersonId = 0, lenPersonId = 0;

    int i = 0;

    // 1. Все выделяем дескрипторы среды и подключения
    retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
    CHECK_ERROR(retcode, "SQLAllocHandle(ENV)", SQL_HANDLE_ENV);

    retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    CHECK_ERROR(retcode, "SQLAllocHandle(DBC)", SQL_HANDLE_DBC);

    // 2. Устанавливаем версию ODBC
    retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0);
    CHECK_ERROR(retcode, "SQLSetEnvAttr(ODBC_VERSION)", SQL_HANDLE_ENV);

    // 3. Создаём оператор
    retcode = SQLAllocStmt(hdbc, &hstmt);
    CHECK_ERROR(retcode, "SQLAllocStmt(HSTMT)", SQL_HANDLE_STMT);

    // 4. Выполняем запрос SELECT
    retcode = SQLExecDirect(hstmt, (SQLCHAR*)"SELECT * FROM Persons WHERE PersonID = ?", SQL_NTS);
    CHECK_ERROR(retcode, "SQLExecDirect(SELECT)", SQL_HANDLE_STMT);

    // 5. Связываем столбцы результата с переменными C
    retcode = SQLBindCol(hstmt, 1, SQL_C_CHAR, strFirstName, LASTNAME_LEN, &lenFirstName);
    CHECK_ERROR(retcode, "SQLBindCol(1, SQL_C_CHAR)", SQL_HANDLE_STMT);

    retcode = SQLBindCol(hstmt, 2, SQL_C_CHAR, strLastName, LASTNAME_LEN, &lenLastName);
    CHECK_ERROR(retcode, "SQLBindCol(2, SQL_C_CHAR)", SQL_HANDLE_STMT);

    retcode = SQLBindCol(hstmt, 3, SQL_C_CHAR, strAddress, ADDRESS_LEN, &lenAddress);
    CHECK_ERROR(retcode, "SQLBindCol(3, SQL_C_CHAR)", SQL_HANDLE_STMT);

    retcode = SQLBindCol(hstmt, 4, SQL_C_CHAR, strCity, CITY_LEN, &lenCity);
    CHECK_ERROR(retcode, "SQLBindCol(4, SQL_C_CHAR)", SQL_HANDLE_STMT);

    // 6. Извлекаем записи и выводим их
    while ((retcode = SQLFetch(hstmt)) != SQL_NO_DATA) {
        printf("ID: %d, Фамилия: %s, Имя: %s, Адрес: %s, Город: %s
",
               cPersonId, strPersonId, strFirstName, strAddress, strCity);
    }

    // 7. Освобождаем ресурсы
    SQLFreeStmt(hstmt, SQL_DROP);
    SQLDisconnect(hdbc);
    SQLFreeConnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);

    return 0;
}
