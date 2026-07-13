# include <stdio.h>
# include <stdlib.h>
# include <sql.h>
# include <sqlext.h>
 
int main()
{
    HENV henv = NULL;
    HDBC hdbc = NULL;
    SQLRETURN ret;
 
    ret = SQLAllocHandle( SQL_HANDLE_ENV, NULL, &henv );
    if (ret != SQL_SUCCESS)
        puts("no env");

    ret = SQLSetEnvAttr( henv, SQL_ATTR_ODBC_VERSION,
        (void *)SQL_OV_ODBC3, 0 );
    if (ret != SQL_SUCCESS)
        puts("no attr");
 
    ret = SQLAllocHandle( SQL_HANDLE_DBC, henv, &hdbc );
    if (ret != SQL_SUCCESS)
        puts("no handle");

    char *conn = "Driver={ODBC Driver 17 for SQL Server};\
        Server=localhost;\
        Database=msdb;Uid=sa;Pwd=_V0cabular;\
        TrustServerCertificate=yes";
 
    ret = SQLDriverConnect(hdbc, 0,
        (SQLCHAR *) conn, SQL_NTS,
        (SQLCHAR *) NULL, SQL_NTS,
        0, SQL_DRIVER_NOPROMPT);
    SQLCHAR szSqlState; // Буфер для SQLSTATE (5 символов)
    SQLINTEGER pfNativeError; // Нативный код ошибки
    SQLCHAR szErrorMsg[256];
    
    if (ret != SQL_SUCCESS)
        ret = SQLError(henv, hdbc, NULL
            , szSqlState, &pfNativeError, szErrorMsg
            , sizeof(szErrorMsg), NULL);
    puts(szErrorMsg);
    SQLDisconnect(hdbc);
    SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
    SQLFreeHandle(SQL_HANDLE_ENV, henv);
 
    return EXIT_SUCCESS;
}