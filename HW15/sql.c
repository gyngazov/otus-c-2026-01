#include <stdio.h>
#include <sqlite3.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "sqlite.h"

int main() { 

    struct Params params;
    get_params(&params);
    
    if (strncmp(params.type, "sqlite", 6) != 0 || strlen(params.type) != 6) {
        puts("Wrong bd type.");
        exit(EXIT_FAILURE);
    }

    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = init(params, &db, &stmt);
  
    if (rc == ERR_NOT_OPEN)
        exit(rc);
    else if (rc == ERR_PREP_QRY || rc == ERR_PREP_STMT)
        goto db;
    else if (rc != 0)
        goto st;

    rc = EXIT_FAILURE;
    const char *tmp = "select %s \
                        from %s \
                        where %s is not null;";  
    char sel[QRY_LEN];
    if (snprintf(sel, QRY_LEN, tmp, 
        params.column, params.table, params.column) < 0) {
        puts("Query construction error.");
        goto st;
    }
    
    int rec = sqlite3_prepare_v2(db, sel, -1, &stmt, NULL);
    if (rec != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", 
            sqlite3_errmsg(db));
        goto db;
    }

    struct List *rows = select_column(stmt);
    
    if (rows == NULL)
        goto st;

    const int count = rows->len;
    int s1 = 0, s2 = 0;
    int min = rows->arr[0];
    int max = min; 
    int r;
    for (int i = 0; i < count; i++) {
        r = rows->arr[i];
        s1 += r;
        s2 += r * r;
        if (r < min)
            min = r;
        if (r > max)
            max = r;
    }
    FREE(rows->arr);
    FREE(rows);
    const double avg = (double) s1 / count; 
    const double dev = sqrt((s2 - (double) s1 * s1 / count) / count);
    puts("STATISTICS");
    printf("tip: %s\ndbs: %s\ntab: %s\ncol: %s\n",
        params.type, params.db, params.table, params.column);
    printf("cnt: %d\nmin: %d\nmax: %d\nsum: %d\navg: %.4f\ndev: %.4f\n", 
        count, min, max, s1, avg, dev);
    rc = EXIT_SUCCESS;
st:
    sqlite3_finalize(stmt);
db:
    sqlite3_close(db);
    return rc;
}

// int init(struct Params params, sqlite3 **db, sqlite3_stmt **stmt)
// {
//     int rc = sqlite3_open_v2(params.db, db, SQLITE_OPEN_READWRITE, NULL);

//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(*db));
//         return -1;
//     }

//     const char *tmp = "select typeof(%s) from %s \
//                         where %s is not null \
//                         limit 1;";
//     char sel[QRY_LEN];
//     if (snprintf(sel, QRY_LEN, tmp, 
//         params.column, params.table, params.column) < 0) {
//         puts("Query construction error.");
//         return -2;
//     }
//     rc = sqlite3_prepare_v2(*db, sel, -1, stmt, NULL);
//     if (rc != SQLITE_OK) {
//         fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(*db));
//         return -2;
//     }

//     if ((rc = sqlite3_step(*stmt)) == SQLITE_ROW) {
//         const char *col_t = sqlite3_column_text(*stmt, 0);
//         int len = strlen(col_t);
//         if (len > 7)
//             len = 7;
//         if (strncmp(col_t, "integer", len) != 0) {
//             printf("Column type %s not integer.\n", col_t);
//             return -3;
//         }
//     } else {
//         puts("No data1");
//         return -4;
//     }
//     return 0;
// }

// // 1.на общий случай манипуляции данными
// // 2.для разделения выборки из бд и прочей логики

// struct List *select_column(sqlite3_stmt *stmt) 
// {
//     int capacity = 2;
//     int i = 0;  
    
//     int *temp;
//     int *arr = malloc(capacity * sizeof(int));
//     if (arr == NULL) {
//         perror("Initial allocation failed");
//         return NULL;
//     }
//     while (sqlite3_step(stmt) == SQLITE_ROW) {
//         if (i == capacity) {
//             capacity <<= 1;
//             temp = realloc(arr, capacity * sizeof(int));
//             if (temp == NULL) {
//                 perror("Reallocation failed");
//                 goto err;
//             }
//             arr = temp;
//         }
//         arr[i] = sqlite3_column_int(stmt, 0);
//         i++;
//     }
//     if (i == 0) {
//         puts("No data");
//         goto err;
//     }
//     struct List *rows = (struct List *)malloc(sizeof(struct List));
//     if (rows == NULL) {
//         puts("Memory error");
//         goto err;
//     }
//     rows->arr = arr;
//     rows->len = i;
//     return rows;
// err:
//     FREE(arr);
//     return NULL;
// }

