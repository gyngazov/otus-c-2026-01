#include <sqlite3.h>

void execute_query(sqlite3* db, const char* name) {
    sqlite3_stmt* stmt;
    const char* sql = "SELECT id FROM users WHERE name = ?;";
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка подготовки: " << sqlite3_errmsg(db) << std::endl;
        return;
    }
    // Привязка данных
    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    // Выполнение
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::cout << "Найден ID: " << id << std::endl;
    }
    // Очистка
    sqlite3_finalize(stmt);
}