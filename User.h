#pragma once
#include <string>
#include <vector>
struct User {
    std::string username;
    std::string password; 
    int levelProgress; // Уровень прогресса
    int correctRequests; // Количество правильно обработанных запросов
    int totalRequests; // Общее количество запросов
    User(const std::string& user, const std::string& pass)
        : username(user), password(pass), levelProgress(0), correctRequests(0), totalRequests(0) {}
};
