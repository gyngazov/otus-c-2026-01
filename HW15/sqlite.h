#include <sqlite3.h>
#include <stdio.h>

//#include "config.h"

#define QRY_LEN     128
#define FREE(ptr)   do { \
                    free(ptr); \
                    ptr = NULL; \
                    } while (0)

struct List {
    int *arr;
    int len;
};

typedef enum {
    ERR_FREE = 0, 
    ERR_NOT_OPEN,    
    ERR_PREP_QRY,
    ERR_PREP_STMT,
    ERR_COL_NOT_INT,
    ERR_NO_DATA      
} Status;

struct List *select_column(sqlite3_stmt *stmt); 
int init(struct Params params, sqlite3 **db, sqlite3_stmt **stmt);
