#pragma once
#include <sqlite3.h>
#include <string>
#include "User.h"

class Database {
public:
    explicit Database(const std::string& dbName);
    ~Database();

    // Создать таблицу пользователей, если она не существует
    bool createUsersTable();

    // Добавить нового пользователя
    bool createUser(const User& user);

    // Получить пользователя по имени пользователя
    User* getUser(const std::string& username);

    // Обновить прогресс пользователя
    void updateUserProgress(const User& user);
private:
    sqlite3* db = nullptr;
    std::string dbName;
};

