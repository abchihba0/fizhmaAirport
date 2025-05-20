#include <iostream>
#include <string>
#include "Airport.h"
#include "AuthManager.h"
#include "Database.h" 
#include "Airplane.h" 
#include "VPP.h"      
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"

// делаю коммиты делаю коммиты
// делаю коммиты делаю коммиты



int main() {
    // Инициализация базы данных и менеджера авторизации
    Database db("game_progress.db");
    AuthManager auth(db);


    User* user = nullptr;
    std::string choice;

    // Авторизация пользователя
    std::cout << "Welcome to Airport Game!\n";
    while (true) {
        std::cout << "Register (r), Login (l), or Exit (e)? ";
        std::cin >> choice;
        if (choice == "r" || choice == "R") {
            std::string username, password;
            std::cout << "Enter new username: ";
            std::cin >> username;
            std::cout << "Enter new password: ";
            std::cin >> password;
            if (auth.registerUser (username, password)) {
                std::cout << "Registration successful! You can now log in.\n";
            } else {
                std::cout << "Registration failed. Username might already exist.\n";
            }
        } else if (choice == "l" || choice == "L") {
            std::string username, password;
            std::cout << "Username: ";
            std::cin >> username;
            std::cout << "Password: ";
            std::cin >> password;
            user = auth.login(username, password);
            if (user) {
                std::cout << "Login successful!\n";
                break;
            } else {
                std::cout << "Invalid username or password. Try again.\n";
            }
        } else if (choice == "e" || choice == "E") {
            std::cout << "Exiting program.\n";
            return 0;
        } else {
            std::cout << "Unknown option. Please try again.\n";
        }
    }

    // Начало игрового процесса
    Airport port;
    std::cout << "Hello=) This is a game that simulates the work of an airport manager. \n"
                 "During the game you need to process requests coming to the control center.\n"
                 "You can allow the takeoff or landing of the aircraft, or you can send the aircraft on a second circle. \n"
                 "At each level there is a certain number of requests that you will receive, you need to process them all.\n"
                 "To exit to the main menu, you can write exit at any time the game is going on." << std::endl;

    std::string answer;
    std::cout << "Are you ready to go?" << std::endl;
    std::cin >> answer;
    if (answer == "Y" || answer == "y") {
        port.gameProcessing("Begin", 0, db, *user); // Передаем базу данных и пользователя в игру
    }

    // Сохранение прогресса и очистка перед выходом
    
    if (user) {
        db.updateUserProgress(*user);
        delete user;
        user = nullptr;
    }

    return 0;
}
