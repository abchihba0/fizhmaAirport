#pragma once
#include <string>
#include "Database.h"
#include "User.h"

class AuthManager {
public:
    AuthManager(Database& database) : db(database) {}

    // Возвращает указатель на User при успешном входе, nullptr при неудаче
    User* login(const std::string& username, const std::string& password);

    // Создает нового пользователя, возвращает true если успешно
    bool registerUser(const std::string& username, const std::string& password);

private:
    Database& db;
};
