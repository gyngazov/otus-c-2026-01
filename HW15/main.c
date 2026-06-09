#include <stdio.h>
#include <sqlite3.h>
#include <math.h>
#include <stdlib.h>


struct List {
    int *arr;
    int len;
};
struct List *select_column(sqlite3_stmt *stmt); 

int main() { 
    sqlite3 *db;
    sqlite3_stmt *stmt;
    int rc = sqlite3_open("test.db", &db);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return EXIT_FAILURE;
    }

    const char *type = "select typeof(b) == 'integer' from tab limit 1;";
    rc = sqlite3_prepare_v2(db, type, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return EXIT_FAILURE;
    }

    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        if (sqlite3_column_int(stmt, 0) != 1) {
            puts("column is not integer");
            return EXIT_FAILURE;
        }
    } else {
        puts("No data");
        return EXIT_FAILURE;
    }

    const char *sel = "SELECT b from tab;";                
    rc = sqlite3_prepare_v2(db, sel, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return EXIT_FAILURE;
    }

    const struct List *rows = select_column(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    if (rows == NULL)
        return EXIT_FAILURE;
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
    printf("count: %d\nmin: %d\nmax: %d\nsum: %d\navg: %.4f\ndev: %.4f\n", 
        count, min, max, s1, avg, dev);
    return EXIT_SUCCESS;
}

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

