 
   #include "User.h"
   #include "Database.h"
   #include <iostream>

   void registerUser (Database& db) {
       std::string username, password;
       std::cout << "Enter username: ";
       std::cin >> username;
       std::cout << "Enter password: ";
       std::cin >> password;

       User newUser (username, password);
       if (db.createUser (newUser )) {
           std::cout << "User  registered successfully!" << std::endl;
       } else {
           std::cout << "Failed to register user. Username may already exist." << std::endl;
       }
   }

   User* loginUser (Database& db) {
       std::string username, password;
       std::cout << "Enter username: ";
       std::cin >> username;
       std::cout << "Enter password: ";
       std::cin >> password;

       User* user = db.getUser (username);
       if (user && user->password == password) {
           std::cout << "Login successful!" << std::endl;
           return user;
       } else {
           std::cout << "Invalid username or password." << std::endl;
           return nullptr;
       }
   }
   