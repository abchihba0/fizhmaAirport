#include "Airport.h"
#include<iostream>
#include <ctime>
#include <chrono>  // для времени
#include <thread>  // для sleep_for
#include <algorithm>
#include <queue>
#include "Database.h"
#include "User.h"


Airport::Airport() {
    // Длины, требуемые для каждого типа самолёта (в метрах)
    const std::vector<int> requiredLengths = {
        CargoPlane::NEED_VPP_LENGTH,
        PassengerPlane::NEED_VPP_LENGTH,
        AgriculturePlane::NEED_VPP_LENGTH,
        MilitaryPlane::NEED_VPP_LENGTH,
        BusinessPlane::NEED_VPP_LENGTH,
        RescuePlane::NEED_VPP_LENGTH
    };

    // Инициализация MemoryAboutLevelsProgress
    for (int i = 0; i < countOfReQuestsOnTheLevel.size(); ++i) {
        MemoryAboutLevelsProgress.push_back(new LevelProgress());
        MemoryAboutLevelsProgress[i]->Level = (i+1); // забиваем лвл в переменную каждому представителю LevelProgress
        MemoryAboutLevelsProgress[i]->vpp_count = runwaysPerLevel[i]; // забиваем колво полос в переменную каждому представителю LevelProgress
    }

    // Генерация полос для каждого уровня
    for (int level = 0; level < countOfReQuestsOnTheLevel.size(); ++level) {
        int runwaysNeeded = runwaysPerLevel[level];
        std::vector<int> lengths;
        std::vector<int> remainingLengths = requiredLengths;

        // Инициализация генератора случайных чисел
        std::srand(static_cast<unsigned int>(std::time(nullptr)));

        // Генерация полос для текущего уровня
        for (int i = 0; i < runwaysNeeded; ++i) {
            bool canGenerateRunway = false;
            int randomLength;

            do {
                canGenerateRunway = false;
                if (!remainingLengths.empty()) {
                    int randomIndex = std::rand() % remainingLengths.size();
                    randomLength = remainingLengths[randomIndex] + (std::rand() % 5) * 100;
                } else {
                    int minLength = *std::min_element(requiredLengths.begin(), requiredLengths.end());
                    randomLength = minLength + (std::rand() % 10) * 100;
                }

                for (int len : requiredLengths) {
                    if (randomLength >= len) {
                        canGenerateRunway = true;
                        break;
                    }
                }

                if (canGenerateRunway) {
                    for (auto it = remainingLengths.begin(); it != remainingLengths.end(); ) {
                        if (randomLength >= *it) {
                            it = remainingLengths.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }
            } while (!canGenerateRunway);

            lengths.push_back(randomLength);
            MemoryAboutLevelsProgress[level]->vpps.push_back(new VPP(randomLength));
        }

        if (level > 0) {
            MemoryAboutLevelsProgress[level]->vpps = MemoryAboutLevelsProgress[level-1]->vpps;
            for (int i = MemoryAboutLevelsProgress[level-1]->vpps.size(); i < runwaysNeeded; ++i) {
                MemoryAboutLevelsProgress[level]->vpps.push_back(new VPP(lengths[i]));
            }
        }
    }
}


Airplane* Airport::set_manager(LevelProgress* ourLevel){

    std::vector<Airplane*> typesOfPlanesToGenerate = { //массив, состоящий из всех наших типов
      new CargoPlane(),
      new PassengerPlane(),
      new AgriculturePlane(),
      new MilitaryPlane(),
      new BusinessPlane(),
      new RescuePlane()
    };

    bool can_we_generate_this_type;//флажок, можем ли мы сгенерировать данный тип самолета
    int count_of_generated_types = 6;//кол-во возможных типов самолета
    int random_type;//случайный индекс типа самолета

    do {
      can_we_generate_this_type = false;
      random_type = rand() % count_of_generated_types;

        for(auto vpp : ourLevel->vpps) {//бегаем по дорожкам 
            if (vpp->get_lenght() >= typesOfPlanesToGenerate[random_type]->getVppLength()) {//если находится нужная
                if(vpp->getBusyTime() < typesOfPlanesToGenerate[random_type]->getRequiredTime()*typesOfPlanesToGenerate[random_type]->getMaxCircle()) {
                    can_we_generate_this_type = true;
                    break;
                }
            }
        }
      if (!can_we_generate_this_type)//если не находится, то присваиваем данному индексу тип самолета в конце вектора и уменьшаем колв-в
        typesOfPlanesToGenerate[random_type] = typesOfPlanesToGenerate[--count_of_generated_types];

    } while (!can_we_generate_this_type&&count_of_generated_types!=0);

    if (can_we_generate_this_type)
      return typesOfPlanesToGenerate[random_type];
    else
      return nullptr;
  }


void Airport::game(LevelProgress* ourLevel, Database& db, User& user)
{
    srand(time(0));
    std::cout << "You have " << ourLevel->vpp_count << " vpps on " << ourLevel->Level << " level" << std::endl;
    double percentageOfCurrentLevelPassed = 0.0;
    int requests=ourLevel->countOfProcessedRequests;
    std::vector<Airplane*> tempmanager;
    int result;
    std::vector<Airplane*> PlanesWhoFlewInFromTheCircle;
    std::vector<Airplane*> PlanesWhoWantToFly;
    for(int i = 0; i < countOfReQuestsOnTheLevel[ourLevel->Level-1] - requests; i++) { 
        for (auto vpp : ourLevel->vpps) {
            if (vpp->getBusyTime() > 0) {
                vpp->setBusyTime(vpp->getBusyTime() - 1);
            }
        }
        if(!ourLevel->managerLanding.empty()) {
            while(!ourLevel->managerLanding.empty()) {
                tempmanager.push_back(ourLevel->managerLanding.top());
                tempmanager.back()->setTime(tempmanager.back()->getTime()-1);
                ourLevel->managerLanding.pop();
            }
            for(auto plane : tempmanager) {
                ourLevel->managerLanding.push(plane);
            } 
            tempmanager.resize(0);
            PlanesWhoFlewInFromTheCircle.resize(0);
            while(!ourLevel->managerLanding.empty() && ourLevel->managerLanding.top()->getTime() == 0) {
                PlanesWhoFlewInFromTheCircle.push_back(ourLevel->managerLanding.top());
                ourLevel->managerLanding.pop();
            }

            if(PlanesWhoFlewInFromTheCircle.size() != 0) {
                std::cout << std::endl << "We have " << PlanesWhoFlewInFromTheCircle.size() << " planes who flew in from the circle" << std::endl;
                std::cout << "you must send it to the second round or accept it" << std::endl << std::endl;
                for(int i = 0; i < PlanesWhoFlewInFromTheCircle.size(); i++) {
                    result = processing(PlanesWhoFlewInFromTheCircle[i], ourLevel, db, user);
                    if(result == 2) { 
                        std::cout << "Progress saved. Returning to level selection..." << std::endl;
                        return gameProcessing("Begin", ourLevel->Level, db, user);
                    }
                    else if(result == 1) { 
                        ourLevel->countOfCorrectProcessedRequests+=1;
                        ourLevel->countOfProcessedRequests+=1;
                    }
                    else if(result == -1) { 
                        if (PlanesWhoFlewInFromTheCircle[i]->getCircle() == PlanesWhoFlewInFromTheCircle[i]->getMaxCircle()) {
                            std::cout << "It was the last circle" << std::endl;
                        }
                        else {
                            PlanesWhoFlewInFromTheCircle[i]->increaseCircle();
                            PlanesWhoFlewInFromTheCircle[i]->setTime(PlanesWhoFlewInFromTheCircle[i]->getRequiredTime());
                            ourLevel->managerLanding.push(PlanesWhoFlewInFromTheCircle[i]);
                        }
                    }
                    else if(result == -2) { 
                        if (PlanesWhoWantToFly.size() > i && PlanesWhoWantToFly[i]->getCircle() == PlanesWhoWantToFly[i]->getMaxCircle()) {
                            std::cout << "It was the last circle" << std::endl;
                        }
                        else if (PlanesWhoWantToFly.size() > i) {
                            PlanesWhoWantToFly[i]->increaseCircle();
                            PlanesWhoWantToFly[i]->setTime(PlanesWhoWantToFly[i]->getRequiredTime());
                            ourLevel->managerRise.push(PlanesWhoWantToFly[i]);
                        }
                    }
                    else if(result == 0) {
                        ourLevel->countOfProcessedRequests += 1;
                    }
                }
                i--;
            }
            else {
                Airplane *tempPlane = set_manager(ourLevel);
                result = processing(tempPlane, ourLevel, db, user);
                if(result == 2) { 
                    std::cout << "Progress saved. Returning to level selection..." << std::endl;
                    return gameProcessing("Begin", ourLevel->Level, db, user);
                }
                else if(result == 1) { 
                    ourLevel->countOfCorrectProcessedRequests+=1;
                    ourLevel->countOfProcessedRequests+=1;
                }
                else if(result == -1) { 
                    tempPlane->increaseCircle();
                    tempPlane->setTime(tempPlane->getRequiredTime());
                    ourLevel->managerLanding.push(tempPlane);
                }
                else if(result == -2) { 
                    tempPlane->increaseCircle();
                    tempPlane->setTime(tempPlane->getRequiredTime());
                    ourLevel->managerRise.push(tempPlane);
                }
                else if(result == 0) {
                    ourLevel->countOfProcessedRequests += 1;
                }
            }
        }
        if(!ourLevel->managerRise.empty()) {
            while(!ourLevel->managerRise.empty()) {
                tempmanager.push_back(ourLevel->managerRise.top());
                tempmanager.back()->setTime(tempmanager.back()->getTime()-1);
                ourLevel->managerRise.pop();
            }
            for(auto plane : tempmanager) {
                ourLevel->managerRise.push(plane);
            } 
            tempmanager.resize(0);
            PlanesWhoWantToFly.resize(0);
            while(!ourLevel->managerRise.empty() && ourLevel->managerRise.top()->getTime() == 0) {
                PlanesWhoWantToFly.push_back(ourLevel->managerRise.top());
                ourLevel->managerRise.pop();
            }
            if(PlanesWhoWantToFly.size() != 0) {
                std::cout << std::endl << "We have " << PlanesWhoWantToFly.size() << " planes that want to take off" << std::endl;
                std::cout << "you must send it to the second round or accept it" << std::endl << std::endl;
                for(int i = 0; i < PlanesWhoWantToFly.size(); i++) {
                    result = processing(PlanesWhoWantToFly[i], ourLevel, db, user);
                    if(result == 2) { 
                        std::cout << "Progress saved. Returning to level selection..." << std::endl;
                        return gameProcessing("Begin", ourLevel->Level, db, user);
                    }
                    else if(result == 1) { 
                        ourLevel->countOfCorrectProcessedRequests+=1;
                        ourLevel->countOfProcessedRequests+=1;
                    }
                    else if(result == -1) { 
                        if (PlanesWhoFlewInFromTheCircle.size() > i && PlanesWhoFlewInFromTheCircle[i]->getCircle() == PlanesWhoFlewInFromTheCircle[i]->getMaxCircle()) {
                            std::cout << "It was the last circle" << std::endl;
                        }
                        else if (PlanesWhoFlewInFromTheCircle.size() > i) {
                            PlanesWhoFlewInFromTheCircle[i]->increaseCircle();
                            PlanesWhoFlewInFromTheCircle[i]->setTime(PlanesWhoFlewInFromTheCircle[i]->getRequiredTime());
                            ourLevel->managerLanding.push(PlanesWhoFlewInFromTheCircle[i]);
                        }
                    }
                    else if(result == -2) { 
                        if (PlanesWhoWantToFly[i]->getCircle() == PlanesWhoWantToFly[i]->getMaxCircle()) {
                            std::cout << "It was the last circle" << std::endl;
                        }
                        else {
                            PlanesWhoWantToFly[i]->increaseCircle();
                            PlanesWhoWantToFly[i]->setTime(PlanesWhoWantToFly[i]->getRequiredTime());
                            ourLevel->managerRise.push(PlanesWhoWantToFly[i]);
                        }
                    }
                    else if(result == 0) {
                        ourLevel->countOfProcessedRequests += 1;
                    }
                }
                i--;
            }
            else {
                Airplane *tempPlane = set_manager(ourLevel);
                result = processing(tempPlane, ourLevel, db, user);
                if(result == 2) { 
                    std::cout << "Progress saved. Returning to level selection..." << std::endl;
                    return gameProcessing("Begin", ourLevel->Level, db, user);
                }
                else if(result == 1) { 
                    ourLevel->countOfCorrectProcessedRequests+=1;
                    ourLevel->countOfProcessedRequests+=1;
                }
                else if(result == -1) { 
                    tempPlane->increaseCircle();
                    tempPlane->setTime(tempPlane->getRequiredTime());
                    ourLevel->managerLanding.push(tempPlane);
                }
                else if(result == -2) { 
                    tempPlane->increaseCircle();
                    tempPlane->setTime(tempPlane->getRequiredTime());
                    ourLevel->managerRise.push(tempPlane);
                }
                else if(result == 0) {
                    ourLevel->countOfProcessedRequests += 1;
                }
            }
        }
        else {
            Airplane *tempPlane = set_manager(ourLevel);
            result = processing(tempPlane, ourLevel, db, user);

            if(result == 2) { 
                std::cout << "Progress saved. Returning to level selection..." << std::endl;
                return gameProcessing("Begin", ourLevel->Level, db, user);
            }
            else if(result == 1) { 
                ourLevel->countOfCorrectProcessedRequests+=1;
                ourLevel->countOfProcessedRequests+=1;
            }
            else if(result == -1) { 
                tempPlane->increaseCircle();
                tempPlane->setTime(tempPlane->getRequiredTime());
                ourLevel->managerLanding.push(tempPlane); 
            }
            else if(result == -2) { 
                tempPlane->increaseCircle();
                tempPlane->setTime(tempPlane->getRequiredTime());
                ourLevel->managerRise.push(tempPlane);
            }
            else if(result == 0) {
                ourLevel->countOfProcessedRequests += 1;
            }
        }
    }

    gameProcessing("LevelComplete", ourLevel->Level, db, user);
}


int Airport::processing(Airplane *tempPlane, LevelProgress* ourLevel, Database& db, User& user)
{
    if (!tempPlane) {
        std::cerr << "Error: tempPlane is nullptr in processing()" << std::endl;
        return 0;
    }

    int typeOfRequest = rand() % 2;
    std::cout << "Type of request Plane: " << tempPlane->getType() << "  Request type: " << ((typeOfRequest==0) ? "landing " : "takeoff ") << " Max Circle: "
    << tempPlane->getMaxCircle() << "  Need Length: " << tempPlane->getVppLength() << "  Time for next circle: " << tempPlane->getRequiredTime() << "  Current circle: " << tempPlane->getCircle() << std::endl;
    std::cout << "busyness of vpp: " << std::endl;
    for(int i = 0; i < ourLevel->vpp_count; i++) {
        std::cout << "vpp number " << i+1 << ": " << "time busy: " << ourLevel->vpps[i]->getBusyTime() << ", length: " << ourLevel->vpps[i]->get_lenght() << std::endl;
    }

    int choice = 1000;
    std::string input;
    int tempVpp;
    std::string vppInput;
    while (choice != -1 && choice != 0 && (choice < 1 || choice > ourLevel->vpp_count))
    {
        std::cin >> input;
        if (input == "exit") {
            user.levelProgress = ourLevel->Level;
            user.correctRequests = ourLevel->countOfCorrectProcessedRequests;
            user.totalRequests = ourLevel->countOfProcessedRequests;
            db.updateUserProgress(user);
            return 2;
        }
        try {
            choice = std::stoi(input);
        } catch (...) {
            std::cout << "Wrong format." << std::endl;
            continue;
        }
        if(choice > 0 && choice <= ourLevel->vpp_count) {
            tempVpp = choice;
            while(ourLevel->vpps[tempVpp-1]->getBusyTime() != 0 || ourLevel->vpps[tempVpp-1]->get_lenght() < tempPlane->getVppLength()) {
                if(ourLevel->vpps[tempVpp-1]->getBusyTime() != 0) {
                    std::cout << "This vpp is busy. Please, choose another vpp." << std::endl;
                } else {
                    std::cout << "Unfortunately, you cannot accept the aircraft on this runway because its length is not sufficient. Please, choose another vpp." << std::endl;
                }
                std::cin >> vppInput;
                if(vppInput == "exit") {
                    user.levelProgress = ourLevel->Level;
                    user.correctRequests = ourLevel->countOfCorrectProcessedRequests;
                    user.totalRequests = ourLevel->countOfProcessedRequests;
                    db.updateUserProgress(user);
                    return 2;
                }
                try {
                    tempVpp = std::stoi(vppInput);
                } catch (...) {
                    std::cout << "Wrong format." << std::endl;
                    continue;
                }
            }
            ourLevel->vpps[tempVpp-1]->setBusyTime(tempPlane->getRequiredTime());
            std::cout << "Successful request processing" << std::endl;
            return 1;
        }
        else if(choice == -1 && typeOfRequest == 0) {
            if (!(tempPlane->getCircle()==tempPlane->getMaxCircle())) {
                std::cout << "Sending the plane on a second circle..." << std::endl;
            }
            return -1;
        }
        else if(choice == -1 && typeOfRequest == 1) {
            if (!(tempPlane->getCircle()==tempPlane->getMaxCircle())) {
                std::cout << "Sending the plane on a second circle..." << std::endl;
            }
            return -2;
        }
        else if(choice == 0) {
            std::cout << "Skipping this request..." << std::endl;
            return 0;
        }
        else {
            std::cout << "Wrong format." << std::endl;
            continue;
        }
    }
    return 0;
}


void Airport::gameProcessing(std::string point, int tempLvl, Database& db, User& user)
{	
    if(point == "Begin") {
        std::cout << "To accept the request, you need to press the number of vpp. \nThe request will be skipped when the timer expires \nTo send the plane on a next circle or ask it to wait for takeoff, you need to press -1." << std::endl;

        std::cout << "Available levels:" << std::endl;
        for(int i = 0; i < countOfReQuestsOnTheLevel.size(); ++i) {
            std::cout << "Level " << (i+1) << " - ";
            if(MemoryAboutLevelsProgress[i]->countOfProcessedRequests > 0) {
                std::cout << "Progress: " << MemoryAboutLevelsProgress[i]->countOfCorrectProcessedRequests 
                        << "/" << countOfReQuestsOnTheLevel[i];
                if(i > 0) {
                    double prevProgress = (double)MemoryAboutLevelsProgress[i-1]->countOfCorrectProcessedRequests / 
                                        countOfReQuestsOnTheLevel[i-1];
                    if(prevProgress < 0.75) {
                        std::cout << " (Locked - complete 75% of previous level)";
                    }
                }
                std::cout << std::endl;
            }
            else {
                if(i > 0) {
                    double prevProgress = (double)MemoryAboutLevelsProgress[i-1]->countOfCorrectProcessedRequests / 
                                        countOfReQuestsOnTheLevel[i-1];
                    if(prevProgress < 0.75) {
                        std::cout << "Locked - complete 75% of level " << i << std::endl;
                        continue;
                    }
                }
                std::cout << "Not started" << std::endl;
            }
        }

        std::cout << "Which Level do you want to pass?" << std::endl;
        std::string input;
        std::cin >> input;
        if(input == "exit") {
            std::cout << "Exiting game..." << std::endl;
            return;
        }
        int tempLvl=std::stoi(input);

        if(tempLvl < 1 || tempLvl > countOfReQuestsOnTheLevel.size()) {
            std::cout << "Invalid level number! Please enter a number between 1 and " << countOfReQuestsOnTheLevel.size() << "." << std::endl;
            return gameProcessing("Begin", 0, db, user);
        }

        if(tempLvl > 1) {
            LevelProgress* prevLevel = MemoryAboutLevelsProgress[tempLvl-2];
            double progress = (double)prevLevel->countOfCorrectProcessedRequests / 
                            countOfReQuestsOnTheLevel[tempLvl-2];

            if(progress < 0.75) {
                std::cout << "You need to complete at least 75% of level " << (tempLvl-1) << " before accessing this level!" << std::endl;
                return gameProcessing("Begin", 0, db, user);
            }
        }

        if(MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests > 0) {
            if(MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests < countOfReQuestsOnTheLevel[tempLvl-1]) {
                std::cout << "You have already started this level before, but did not finish it. Do you want to continue(C) or start over(S)? Your progress will be overwritten" << std::endl;
                std::string answer;
                std::cin >> answer;
                if(answer == "C" || answer == "c") {
                    game(MemoryAboutLevelsProgress[tempLvl-1], db, user);
                }
                else if(answer == "S" || answer == "s") {
                    for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
                        MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
                    }
                    while(!MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.empty()) {
                        MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.pop();
                    }
                    MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
                    MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
                    game(MemoryAboutLevelsProgress[tempLvl-1], db, user);
                }
                else if(answer == "exit") {
                    return gameProcessing("Begin", 0, db, user);
                }
            }
            else {
                std::cout << "You have already completed this level before. Your result: " 
                        << MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests
                        << "/" << countOfReQuestsOnTheLevel[tempLvl-1] 
                        << ". Do you want to replay the level(Y/n)? Your current result will be overwritten." << std::endl;
                std::string answer;
                std::cin >> answer;
                if(answer == "Y" || answer == "y") {
                    for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
                        MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
                    }
                    while(!MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.empty()) {
                        MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.pop();
                    }
                    MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
                    MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
                    game(MemoryAboutLevelsProgress[tempLvl-1], db, user);
                }
                else {
                    return gameProcessing("Begin", 0, db, user);
                }
            }
        }
        else {
            game(MemoryAboutLevelsProgress[tempLvl-1], db, user);
        }
    }
    else if(point == "LevelComplete") {
        LevelProgress* completedLevel = MemoryAboutLevelsProgress[tempLvl-1];
        int correct = completedLevel->countOfCorrectProcessedRequests;
        int total = countOfReQuestsOnTheLevel[tempLvl-1];
        std::string answer;
        double percent = double(correct) / double(total);
        if(percent >= 0.75) {
            if(tempLvl != countOfReQuestsOnTheLevel.size()) {
                std::cout << "Nice work! You have " << correct << "/" << total << " passed requests. You can go to the next level(n) or improve your result(i)." << std::endl;
                std::cin >> answer;
                if(answer == "n" || answer == "N") {
                    if(MemoryAboutLevelsProgress[tempLvl]->countOfProcessedRequests > 0) {
                        std::cout << "Next level already has progress: " 
                                << MemoryAboutLevelsProgress[tempLvl]->countOfCorrectProcessedRequests 
                                << "/" << countOfReQuestsOnTheLevel[tempLvl] 
                                << ". Are you sure you want to overwrite it? (Y/n)" << std::endl;
                        std::cin >> answer;
                        if(answer == "Y" || answer == "y") {
                            for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
                                MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
                            }
                            while(!MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.empty()) {
                                MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.pop();
                            }
                            MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
                            MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
                        }
                        else if(answer == "exit") {
                            return gameProcessing("Begin", 0, db, user);
                        }
                    }
                    game(MemoryAboutLevelsProgress[tempLvl], db, user);
                }
                else if(answer == "I" || answer == "i") {
                    for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
                        MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
                    }
                    while(!MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.empty()) {
                        MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.pop();
                    }
                    MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
                    MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
                    game(MemoryAboutLevelsProgress[tempLvl-1], db, user);
                }
                else if(answer == "exit") {
                    return gameProcessing("Begin", 0, db, user);
                }
            }
            else {
                if(correct == total) {
                    std::cout << "You have max result!" << std::endl;
                    gameProcessing("Begin", 0, db, user);
                }
                else {
                    std::cout << "Nice work! You have " << correct << "/" << total << " passed requests. You can improve the result. Will you go(Y/n)?" << std::endl;
                    std::cin >> answer;
                    if(answer == "Y" || answer == "y") {
                        for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
                            MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
                        }
                        while(!MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.empty()) {
                            MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.pop();
                        }
                        MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
                        MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
                        game(MemoryAboutLevelsProgress[tempLvl-1], db, user);
                    }
                    else {
                        return gameProcessing("Begin", 0, db, user);
                    }
                }
            }
        }
        else {
            std::cout << "You need to replay this level or choose another. Replay? (Y/N)" << std::endl;
            std::string answer;
            std::cin >> answer;
            if(answer == "Y" || answer == "y") {
                for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
                    MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
                }
                while(!MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.empty()) {
                    MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.pop();
                }
                MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
                MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
                game(MemoryAboutLevelsProgress[tempLvl-1], db, user);
            }
            else {
                return gameProcessing("Begin", 0, db, user);
            }
        }
        user.levelProgress = tempLvl;
        user.correctRequests = MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests;
        user.totalRequests = countOfReQuestsOnTheLevel[tempLvl-1];
        db.updateUserProgress(user);
    }
}
