# include <stdio.h>
# include <stdlib.h>
# include <sql.h>
# include <sqlext.h>
 
main()
{
    HENV henv = NULL;
    HDBC hdbc = NULL;
    SQLRETURN ret;
 
    /* Initialize the ODBC environment handle. */
    ret = SQLAllocHandle( SQL_HANDLE_ENV, NULL, &henv );
    if (ret != SQL_SUCCESS)
        puts("no env");
    /* Set the ODBC version to version 3 (the highest version) */
    ret = SQLSetEnvAttr( henv, SQL_ATTR_ODBC_VERSION,
        (void *)SQL_OV_ODBC3, 0 );
    if (ret != SQL_SUCCESS)
        puts("no attr");
 
    /* Allocate the connection handle. */
    ret = SQLAllocHandle( SQL_HANDLE_DBC, henv, &hdbc );
    if (ret != SQL_SUCCESS)
        puts("no handle");
 
    /*
    ** Fill the connection string with the minimum
    ** connection information.
    */
    char *conn = "Server=localhost;\
        Database=msdb;User Id=sa;Password=_V0cabular";
 
    /* Connect to the database using the connection string. */
    ret = SQLDriverConnect( hdbc,    /* Connection handle */
        0,                     /* Window handle */
        (SQLCHAR *) conn,         /* Connection string */
        SQL_NTS,               /* This is a null-terminated string */
        (SQLCHAR *)NULL,       /* Output (result) connection string */
        SQL_NTS,               /* This is a null-terminated string */
        0,                     /* Length of output connect string */
        SQL_DRIVER_NOPROMPT ); /* Don't display a prompt window */
 
    if (ret != SQL_SUCCESS)
        puts("no drv");
    /* Disconnect from the database. */
    SQLDisconnect( hdbc );
 
    /* Free the connection handle. */
    SQLFreeHandle( SQL_HANDLE_DBC, hdbc );
 
    /* Free the environment handle. */
    SQLFreeHandle( SQL_HANDLE_ENV, henv );
 
    /* Exit this program. */
    return(0);
}