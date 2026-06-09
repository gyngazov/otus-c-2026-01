#include <stdio.h>
#include <sqlite3.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

struct List {
    int *arr;
    int len;
};
struct List *select_column(sqlite3_stmt *stmt); 
int init(struct Params params, sqlite3 *db, sqlite3_stmt *stmt);

int main() { 

    struct Params params;
    int ret = get_params(&params);
    if (ret)
        exit(EXIT_FAILURE);

    if (strncmp(params.type, "sqlite", 6) != 0 || strlen(params.type) != 6) {
        puts("Wrong bd type.");
        exit(EXIT_FAILURE);
    }

    sqlite3 *db;
    sqlite3_stmt *stmt;
    if (init(params, db, stmt) != 0)
        exit(EXIT_FAILURE);

    struct List *rows = select_column(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    if (rows == NULL)
        exit(EXIT_FAILURE);
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
    free(rows->arr);
    free(rows);
    const double avg = (double) s1 / count; 
    const double dev = sqrt((s2 - (double) s1 * s1 / count) / count);
    puts("STATISTICS");
    printf("type: %s\ndb: %s\ntable: %s\ncolumn: %s\n",
        params.type, params.db, params.table, params.column);
    printf("count: %d\nmin: %d\nmax: %d\nsum: %d\navg: %.4f\ndev: %.4f\n", 
        count, min, max, s1, avg, dev);
    return EXIT_SUCCESS;
}

int init(struct Params params, sqlite3 *db, sqlite3_stmt *stmt)
{
    int rc = sqlite3_open_v2(params.db, &db, SQLITE_OPEN_READWRITE, NULL);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    const char *type = "select typeof(b) from tab limit 1;";
    rc = sqlite3_prepare_v2(db, type, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -2;
    }
    char *col_t;
    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        col_t = sqlite3_column_text(stmt, 0);
    } else {
        puts("No data");
        return -4;
    }

    const char *tmp = "select %s from %s;";  
    char sel[128];
    if (snprintf(sel, 128, "select %s from %s;", params.column, params.table) < 0) {
        puts("Query construction error.");
        return -5;
    }              
    rc = sqlite3_prepare_v2(db, sel, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -6;
    }
    return 0;
}

// 1.на общий случай манипуляции данных 
// 2.для разделения работы с бд и логики обработки
struct List *select_column(sqlite3_stmt *stmt) 
{
    int capacity = 2;
    int i = 0;  
    
    int rc, *temp;
    int *arr = malloc(capacity * sizeof(int));
    if (arr == NULL) {
        perror("Initial allocation failed");
        return NULL;
    }
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (i == capacity) {
            capacity <<= 1;
            temp = realloc(arr, capacity * sizeof(int));
            if (temp == NULL) {
                perror("Reallocation failed");
                free(arr);
                return NULL;
            }
            arr = temp;
        }
        arr[i] = sqlite3_column_int(stmt, 0);
        i++;
    }
    if (i == 0) {
        puts("No data");
        return NULL;
    }
    struct List *rows = (struct List *)malloc(sizeof(struct List));
    rows->arr = arr;
    rows->len = i;
    return rows;
}

