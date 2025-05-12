#include "AuthManager.h"
#include <iostream>
#include <algorithm>

bool AuthManager::registerUser(const std::string& username, const std::string& password) {
    // Validate input
    if (username.empty() || password.empty()) {
        std::cerr << "Username and password cannot be empty\n";
        return false;
    }
    
    if (usernameExists(username)) {
        std::cerr << "Username already exists\n";
        return false;
    }
    
    if (!validatePassword(password)) {
        std::cerr << "Password doesn't meet requirements\n";
        return false;
    }
    
    // Create new user
    User newUser(username, password);
    return database.createUser(newUser);
}

User* AuthManager::login(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) {
        std::cerr << "Username and password cannot be empty\n";
        return nullptr;
    }
    
    User* user = database.getUser(username);
    if (user == nullptr) {
        std::cerr << "User not found\n";
        return nullptr;
    }
    
    if (user->password != password) {
        delete user;
        std::cerr << "Invalid password\n";
        return nullptr;
    }
    
    return user;
}

bool AuthManager::changePassword(const std::string& username, const std::string& oldPassword, const std::string& newPassword) {
    if (!validatePassword(newPassword)) {
        std::cerr << "New password doesn't meet requirements\n";
        return false;
    }
    
    User* user = login(username, oldPassword);
    if (user == nullptr) {
        return false;
    }
    
    user->password = newPassword;
    database.updateUserProgress(*user);
    delete user;
    
    return true;
}

bool AuthManager::validatePassword(const std::string& password) {
    // Basic password validation - can be enhanced
    return password.length() >= 6;
}

bool AuthManager::usernameExists(const std::string& username) {
    User* user = database.getUser(username);
    if (user != nullptr) {
        delete user;
        return true;
    }
    return false;
}