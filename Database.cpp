#include "Database.h"
#include <iostream>

Database::Database(const std::string& dbName) {
    if (sqlite3_open(dbName.c_str(), &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    } else {
        const char* sql = "CREATE TABLE IF NOT EXISTS users ("
                          "username TEXT PRIMARY KEY,"
                          "password TEXT,"
                          "levelProgress INTEGER,"
                          "correctRequests INTEGER,"
                          "totalRequests INTEGER);";
        char* errMsg = nullptr;
        if (sqlite3_exec(db, sql, nullptr, 0, &errMsg) != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
    }
}

Database::~Database() {
    if (db != nullptr) {
        sqlite3_close(db);
    }
}

// Создает нового пользователя в БД
bool Database::createUser (const User& user) {
    if (!db) return false;

    std::string sql = "INSERT INTO users (username, password, levelProgress, correctRequests, totalRequests) VALUES (?, ?, 0, 0, 0);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user.password.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute statement\n";
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

// Получает данные пользователя из БД
User* Database::getUser (const std::string& username) {
    if (!db) return nullptr;

    std::string sql = "SELECT username, password, levelProgress, correctRequests, totalRequests FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    User* user = nullptr;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement\n";
        return nullptr;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        std::string uname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string password = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        int levelProgress = sqlite3_column_int(stmt, 2);
        int correctRequests = sqlite3_column_int(stmt, 3);
        int totalRequests = sqlite3_column_int(stmt, 4);

        user = new User(uname, password);
        user->levelProgress = levelProgress;
        user->correctRequests = correctRequests;
        user->totalRequests = totalRequests;
    } else if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute statement, rc = " << rc << std::endl;
    }

    sqlite3_finalize(stmt);
    return user;
}

// Обновляет прогресс пользователя в БД
void Database::updateUserProgress(const User& user) {
    if (!db) return;

    std::string sql = "UPDATE users SET levelProgress = ?, correctRequests = ?, totalRequests = ? WHERE username = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement\n";
        return;
    }

    sqlite3_bind_int(stmt, 1, user.levelProgress);
    sqlite3_bind_int(stmt, 2, user.correctRequests);
    sqlite3_bind_int(stmt, 3, user.totalRequests);
    sqlite3_bind_text(stmt, 4, user.username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to update user progress\n";
    }

    sqlite3_finalize(stmt);
}




