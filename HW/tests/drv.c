# include <stdio.h>
# include <stdlib.h>
# include <sql.h>
# include <sqlext.h>

void diag_stmt(SQLHSTMT hstmt);

int main()
{
    HENV henv = NULL;
    HDBC hdbc = NULL;
    HSTMT hstmt = NULL;
    SQLRETURN ret;
 
    ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);
    if (ret != SQL_SUCCESS) {
        puts("no env");
        goto err0;
    }

    ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION,
        (void *)SQL_OV_ODBC3, 0);
    if (ret != SQL_SUCCESS) {
        puts("no attr");
        goto err1;
    }
 
    ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
    if (ret != SQL_SUCCESS) {
        puts("no handle");
        goto err1;
    }

    char *conn = "Driver={ODBC Driver 17 for SQL Server};\
        Server=localhost;\
        Database=barkp;Uid=sa;Pwd=_V0cabular;\
        TrustServerCertificate=yes";
 
    ret = SQLDriverConnect(hdbc, 0,
        (SQLCHAR *) conn, SQL_NTS,
        (SQLCHAR *) NULL, SQL_NTS,
        0, SQL_DRIVER_NOPROMPT);
    SQLCHAR szSqlState; // Буфер для SQLSTATE (5 символов)
    SQLINTEGER pfNativeError; // Нативный код ошибки
    SQLCHAR szErrorMsg[256];

    if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
        ret = SQLError(henv, hdbc, NULL
            , szSqlState, &pfNativeError, szErrorMsg
            , sizeof(szErrorMsg), NULL);
        puts(szErrorMsg);
        printf("ret: %d\n", ret);
        goto err2;
    }

    ret = SQLAllocHandle(SQL_HANDLE_STMT, henv, &hstmt);
    if (ret == SQL_ERROR) {
        puts("no stmt");
        goto err2;
    }
    const char *qry = "select 'abc'"; //select id, code ,val from barcodes";
    ret = SQLExecDirect(hstmt, (SQLCHAR *) qry, SQL_NTS);
    if (ret != SQL_SUCCESS) {
        SQLError(henv, hdbc, hstmt
            , szSqlState, &pfNativeError, szErrorMsg
            , sizeof(szErrorMsg), NULL);
        puts(szErrorMsg);
        printf("ret exec: %d\n", ret);
        diag_stmt(hstmt);
        //goto err3;
    }

    SQLLEN cbTest = 0;
    int len = 255;
    SQLWCHAR bff[len];
    //ret = SQLBindCol(hstmt, 1, SQL_C_CHAR, buffer, len, &len);
    ret = SQLBindCol(hstmt, 1, SQL_C_WCHAR, bff, len, &cbTest);
    printf("bind ret: %d\n", ret);
    ret = SQLFetch(hstmt);
    printf("ret: %d , id: %s\n", ret, bff);
        // if (ret == SQL_ERROR) {
        //     puts("err");
        //     break;
        // } else if (ret == SQL_NO_DATA_FOUND) {
        //     puts("data end");
        //     break;
        // }

err3:    
    SQLFreeHandle(SQL_HANDLE_STMT, hstmt);
err2:
    SQLDisconnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
err1:
    SQLFreeHandle(SQL_HANDLE_ENV, henv);
err0:
    return EXIT_FAILURE;
}

void diag_stmt(SQLHSTMT hstmt)
{
    SQLCHAR buffer[SQL_MAX_MESSAGE_LENGTH + 1]; // Буфер для сообщения об ошибке
    SQLCHAR sqlstate[SQL_SQLSTATE_SIZE + 1]; // Буфер для кода SQLSTATE
    SQLINTEGER sqlcode; // Переменная для собственного кода ошибки
    SQLSMALLINT length; // Длина текста сообщения
    SQLSMALLINT i; // Счетчик записей диагностики
    i = 1;
    while (SQLGetDiagRec(SQL_HANDLE_STMT, hstmt, 
        i, sqlstate, &sqlcode, buffer, SQL_MAX_MESSAGE_LENGTH + 1, &length) == SQL_SUCCESS) {
            printf("SQLSTATE: %s", sqlstate);
            printf("Native Error Code: %ld", sqlcode);
            printf("Message: %s", buffer);
            i++;
    }
    
}