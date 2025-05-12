#include "Database.h"
#include <iostream>

Database::Database(const std::string& dbName) : dbName(dbName) {
    if (sqlite3_open(dbName.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    } else {
        createUsersTable();
    }
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::createUsersTable() {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "username TEXT PRIMARY KEY,"
        "password TEXT NOT NULL,"
        "levelProgress INTEGER DEFAULT 0,"
        "correctRequests INTEGER DEFAULT 0,"
        "totalRequests INTEGER DEFAULT 0"
        ");";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error on createUsersTable: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::createUser(const User& user) {
    const char* sql = "INSERT INTO users (username, password, levelProgress, correctRequests, totalRequests) VALUES (?, ?, 0, 0, 0);";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, user.username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user.password.c_str(), -1, SQLITE_STATIC);

    bool success = true;
    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute insert statement: " << sqlite3_errmsg(db) << std::endl;
        success = false;
    }

    sqlite3_finalize(stmt);
    return success;
}

User* Database::getUser(const std::string& username) {
    const char* sql = "SELECT username, password, levelProgress, correctRequests, totalRequests FROM users WHERE username = ?;";
    sqlite3_stmt* stmt = nullptr;
    User* user = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare select statement: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        std::string uname(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        std::string pass(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        int levelProgress = sqlite3_column_int(stmt, 2);
        int correctRequests = sqlite3_column_int(stmt, 3);
        int totalRequests = sqlite3_column_int(stmt, 4);

        user = new User(uname, pass);
        user->levelProgress = levelProgress;
        user->correctRequests = correctRequests;
        user->totalRequests = totalRequests;
    }

    sqlite3_finalize(stmt);
    return user;
}

void Database::updateUserProgress(const User& user) {
    const char* sql = "UPDATE users SET levelProgress = ?, correctRequests = ?, totalRequests = ? WHERE username = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare update statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_int(stmt, 1, user.levelProgress);
    sqlite3_bind_int(stmt, 2, user.correctRequests);
    sqlite3_bind_int(stmt, 3, user.totalRequests);
    sqlite3_bind_text(stmt, 4, user.username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to execute update statement: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
}
