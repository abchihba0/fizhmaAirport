#include <iostream>
#include <string>
#include "Database.h"
#include "User.h"
#include "Airport.h"
#include "Authorization.h"

int main() {
    Database db("game_progress.db");
    AuthManager auth(db);

    User* user = nullptr;
    std::string choice;

    std::cout<<"Welcome to Airport Game!\n";
    while (true) {
        std::cout<<"Register (r), Login (l), or Exit (e)? ";
        std::cin>>choice;
        if (choice == "r" || choice == "R") {
            std::string username, password;
            std::cout<<"Enter new username: ";
            std::cin>>username;
            std::cout<<"Enter new password: ";
            std::cin>> password;
            if (auth.registerUser(username, password)) {
                std::cout <<"Registration successful! You can now log in.\n";
            } else {
                std::cout<< "Registration failed. Username might already exist.\n";
            }
        } else if (choice == "l" || choice == "L") {
            std::string username, password;
            std::cout<< "Username: ";
            std::cin>> username;
            std::cout<< "Password: ";
            std::cin>>password;
            user = auth.login(username, password);
            if (user) {
                std::cout<<"Login successful!\n";
                break;
            } else {
                std::cout << "Invalid username or password. Try again.\n";
            }
        } else if (choice == "e" || choice == "E") {
            std::cout <<"Exiting program.\n";
            return 0;
        } else {
            std::cout <<"Unknown option. Please try again.\n";
        }
    }

    Airport port;
    port.gameProcessing("Begin", 0, db, *user);

    // Save progress and cleanup before exit
    if (user) {
        db.updateUserProgress(*user);
        delete user;
        user = nullptr;
    }

    return 0;
}

