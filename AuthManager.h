#pragma once
#include "Database.h"
#include "User.h"
#include <string>

class AuthManager {
public:
    AuthManager(Database& db) : database(db) {}
    
    bool registerUser(const std::string& username, const std::string& password);
    User* login(const std::string& username, const std::string& password);
    bool changePassword(const std::string& username, const std::string& oldPassword, const std::string& newPassword);

private:
    Database& database;
    
    bool validatePassword(const std::string& password);
    bool usernameExists(const std::string& username);
};