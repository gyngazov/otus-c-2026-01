#include <stdio.h>
#include <sqlite3.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "sqlite.h"
#include "config.h"

int init(struct Params params, sqlite3 **db, sqlite3_stmt **stmt)
{
    int rc = sqlite3_open_v2(params.db, db, SQLITE_OPEN_READWRITE, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
        return ERR_NOT_OPEN;
    }

    const char *tmp = "select typeof(%s) from %s \
                        where %s is not null \
                        limit 1;";
    char sel[QRY_LEN];
    if (snprintf(sel, QRY_LEN, tmp, 
        params.column, params.table, params.column) < 0) {
        puts("Query construction error.");
        return ERR_PREP_QRY;
    }
    rc = sqlite3_prepare_v2(*db, sel, -1, stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(*db));
        return ERR_PREP_STMT;
    }

    if ((rc = sqlite3_step(*stmt)) == SQLITE_ROW) {
        const char *col_t = sqlite3_column_text(*stmt, 0);
        int len = strlen(col_t);
        if (len > 7)
            len = 7;
        if (strncmp(col_t, "integer", len) != 0) {
            printf("Column type %s not integer.\n", col_t);
            return ERR_COL_NOT_INT;
        }
    } else {
        puts("No data");
        return ERR_NO_DATA;
    }
    return ERR_FREE;
}

struct List *select_column(sqlite3_stmt *stmt) 
{
    int capacity = 2;
    int i = 0;  
    
    int *temp;
    int *arr = malloc(capacity * sizeof(int));
    if (arr == NULL) {
        perror("Initial allocation failed");
        return NULL;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        if (i == capacity) {
            capacity <<= 1;
            temp = realloc(arr, capacity * sizeof(int));
            if (temp == NULL) {
                perror("Reallocation failed");
                goto err;
            }
            arr = temp;
        }
        arr[i] = sqlite3_column_int(stmt, 0);
        i++;
    }
    if (i == 0) {
        puts("No data");
        goto err;
    }
    struct List *rows = (struct List *)malloc(sizeof(struct List));
    if (rows == NULL) {
        puts("Memory error");
        goto err;
    }
    rows->arr = arr;
    rows->len = i;
    return rows;
err:
    FREE(arr);
    return NULL;
}