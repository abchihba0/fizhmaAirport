#pragma once
#include <sqlite3.h>
#include <string>
#include "User.h"
class Database {
public:
    Database(const std::string& dbName);
    ~Database();
    bool createUser (const User& user);
    User* getUser (const std::string& username);
    void updateUserProgress(const User& user);
private:
    sqlite3* db;
};