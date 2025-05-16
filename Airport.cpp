
[file name]: Airport.cpp
[file content begin]
#include "Airport.h"
#include<iostream>
#include <ctime>
#include <chrono>  // для времени
#include <thread>  // для sleep_for
#include <algorithm>
#include <queue>
#include <sstream>



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

// основная задача: функцию game нужно доработать. В процессе processing'a запроса мы можем отправить
// самолет на второй круг, в таком случае, у нас пойдет время, которое он будет на втором круге, хотя 
// одновременно с этим другие запросы могут появляться. Пусть мы отправили самолет на второй круг и пошло
// его время на втором круге и  допустим запросы появляются через n минут. ЗАПРОСЫ НЕ МОГУТ ПОЯВЛЯТЬСЯ ЧАЩЕ ЧЕМ
// n МИНУТ, ПОЭТОМУ ЕСЛИ ЧЕРЕЗ ВРЕМЯ n ОСТАНЕТСЯ K < n МИНУТ ДО ПРИЛЕТА САМОЛЕТА С ДОП. КРУГА, ТО САМОЛЕТ СЕЙЧАС НЕ ГЕНЕРИРУЕТСЯ,
// ВРЕМЯ МЕЖДУ ЗАПРОСАМИ БУДЕТ n+K минут. То есть не должно быть такого, что самолет сгенерировался, а потом через секунду прилетит самолет с доп круга. 


void Airport::game(LevelProgress* ourLevel)
{
	std::ostringstream oss;
    std::streambuf* old_cout = std::cout.rdbuf(oss.rdbuf());
	srand(time(0));
	oss << "You have " << ourLevel->vpp_count << " vpps on " << ourLevel->Level << " level" << std::endl;
	double percentageOfCurrentLevelPassed = 0.0; // создаем переменную, в которую после обработки всех запросов запишется процент их прохождения
	// колво правильных запросов равно количеству корректных запросов их сохраненного результата
	int requests=ourLevel->countOfProcessedRequests;
	std::vector<Airplane*> tempmanager; // вектор для временного хранения самолетов с кругами, уменьшенными на 1, для изменения manager
	int result;
	std::vector<Airplane*> PlanesWhoFlewInFromTheCircle; // вектор с прилетевшими с круга самолётами, запросы которых нужно обработать
	std::vector<Airplane*> PlanesWhoWantToFly; // вектор с самолётами, у которых вторичный запрос на взлёт
	for(int i = 0; i < countOfReQuestsOnTheLevel[ourLevel->Level-1] - requests; i++) { // количество запросов которые нужно обработать = количество запросов на уровне - количество уже обработанных запросов
		for (auto vpp : ourLevel->vpps) { // уменьшаем занятость полос
            if (vpp->getBusyTime() > 0) {
                vpp->setBusyTime(vpp->getBusyTime() - 1);
            }
        }
		if(!ourLevel->managerLanding.empty()) {
			// уменьшение времени на некст круге
			while(!ourLevel->managerLanding.empty()) {
				tempmanager.push_back(ourLevel->managerLanding.top());
				tempmanager.back()->setTime(tempmanager.back()->getTime()-1);
				ourLevel->managerLanding.pop();
			}
			for(auto plane : tempmanager) {
				ourLevel->managerLanding.push(plane);
			} 
			tempmanager.resize(0);
			// уменьшили
			// когда уменьшили, смотрим, есть ли самолеты у которых время равно 0. Если есть, добавляем их в PlanesWhoFlewInFromTheCircle и далее обрабатываем
			PlanesWhoFlewInFromTheCircle.resize(0);
			while(ourLevel->managerLanding.top()->getTime() == 0 && !ourLevel->managerLanding.empty()) {
				PlanesWhoFlewInFromTheCircle.push_back(ourLevel->managerLanding.top());
				ourLevel->managerLanding.pop();
			}
			// добавили, теперь обрабатываем, если они есть
			if(PlanesWhoFlewInFromTheCircle.size() != 0) {
				std::cout << std::endl << "We have " << PlanesWhoFlewInFromTheCircle.size() << " planes who flew in from the circle" << std::endl;
				std::cout << "you must send it to the second round or accept it" << std::endl << std::endl;
				for(int i = 0; i < PlanesWhoFlewInFromTheCircle.size(); i++) {
					result = processing(PlanesWhoFlewInFromTheCircle[i], ourLevel);
					if(result == 2) { 
						std::cout << "Progress saved. Returning to level selection..." << std::endl;
						return gameProcessing("Begin", ourLevel->Level);;
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
						
					// обработка запроса
					// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // задержка между запросами в 5 секунд
					// ourLevel->countOfProcessedRequests += 1; // Увеличиваем счетчик обработанных запросов
				}
				i--;
			}
			else {
				Airplane *tempPlane = set_manager(ourLevel);
				result = processing(tempPlane, ourLevel);
				if(result == 2) { 
					std::cout << "Progress saved. Returning to level selection..." << std::endl;
					return gameProcessing("Begin", ourLevel->Level);;
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
			// уменьшение времени на некст круге
			while(!ourLevel->managerRise.empty()) {
				tempmanager.push_back(ourLevel->managerRise.top());
				tempmanager.back()->setTime(tempmanager.back()->getTime()-1);
				ourLevel->managerRise.pop();
			}
			for(auto plane : tempmanager) {
				ourLevel->managerRise.push(plane);
			} 
			tempmanager.resize(0);
			// уменьшили
			// когда уменьшили, смотрим, есть ли самолеты у которых время равно 0. Если есть, добавляем их в PlanesWhoFlewInFromTheCircle и далее обрабатываем
			PlanesWhoWantToFly.resize(0);
			while(ourLevel->managerRise.top()->getTime() == 0 && !ourLevel->managerRise.empty()) {
				PlanesWhoWantToFly.push_back(ourLevel->managerRise.top());
				ourLevel->managerRise.pop();
			}
			// добавили, теперь обрабатываем, если они есть
			if(PlanesWhoWantToFly.size() != 0) {
				std::cout << std::endl << "We have " << PlanesWhoWantToFly.size() << " planes that want to take off" << std::endl;
				std::cout << "you must send it to the second round or accept it" << std::endl << std::endl;
				for(int i = 0; i < PlanesWhoWantToFly.size(); i++) {
					result = processing(PlanesWhoWantToFly[i], ourLevel);
					if(result == 2) { 
						std::cout << "Progress saved. Returning to level selection..." << std::endl;
						return gameProcessing("Begin", ourLevel->Level);;
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
				result = processing(tempPlane, ourLevel);
				if(result == 2) { 
					std::cout << "Progress saved. Returning to level selection..." << std::endl;
					return gameProcessing("Begin", ourLevel->Level);;
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
			result = processing(tempPlane, ourLevel);
	
			if(result == 2) { 
				std::cout << "Progress saved. Returning to level selection..." << std::endl;
				return gameProcessing("Begin", ourLevel->Level);;
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
			// обработка запроса
			// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // задержка между запросами в 5 секунд
			// ourLevel->countOfProcessedRequests += 1; // Увеличиваем счетчик обработанных запросов
		}
	}
	
	// Перенаправляем в gameProcessing для обработки завершения уровня
	gameProcessing("LevelComplete", ourLevel->Level);

	std::cout.rdbuf(old_cout);
}


int Airport::processing(Airplane *tempPlane, LevelProgress* ourLevel)
{
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
			return 2; 
		}
		choice = std::stoi(input);

		if(choice > 0 && choice <= ourLevel->vpp_count) {
			tempVpp = choice;
			while(ourLevel->vpps[tempVpp-1]->getBusyTime() != 0 || ourLevel->vpps[tempVpp-1]->get_lenght() < tempPlane->getVppLength()) {
				if(ourLevel->vpps[tempVpp-1]->getBusyTime() != 0) {
					std::cout << "This vpp is busy. Please, choose another vpp." << std::endl;
				}
				else {
					std::cout << "Unfortunately, you cannot accept the aircraft on this runway because its length is not sufficient. Please, choose another vpp." << std::endl;

				}
				std::cin >> vppInput;
				if (vppInput == "exit") {
					return 2;
				}
				tempVpp = std::stoi(vppInput);
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
}


void Airport::gameProcessing(std::string point, int tempLvl)
{	
	if(point == "Begin") {
		std::cout << "To accept the request, you need to press the number of vpp. \nThe request will be skipped when the timer expires \nTo send the plane on a next circle or ask it to wait for takeoff, you need to press -1." << std::endl;
		
		// Вывод всех доступных уровней с прогрессом
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

		// Проверка корректности ввода уровня
		if(tempLvl < 1 || tempLvl > countOfReQuestsOnTheLevel.size()) {
			std::cout << "Invalid level number! Please enter a number between 1 and " << countOfReQuestsOnTheLevel.size() << "." << std::endl;
			return gameProcessing("Begin", 0);
		}

		// Проверка доступа к уровню (только если выбран не первый уровень)
		if(tempLvl > 1) {
			LevelProgress* prevLevel = MemoryAboutLevelsProgress[tempLvl-2];
			double progress = (double)prevLevel->countOfCorrectProcessedRequests / 
							countOfReQuestsOnTheLevel[tempLvl-2];
			
			if(progress < 0.75) {
				std::cout << "You need to complete at least 75% of level " << (tempLvl-1) << " before accessing this level!" << std::endl;
				return gameProcessing("Begin", 0);
			}
		}

		// Проверка существующего прогресса на выбранном уровне
		if(MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests > 0) {
			if(MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests < countOfReQuestsOnTheLevel[tempLvl-1]) {
				std::cout << "You have already started this level before, but did not finish it. Do you want to continue(C) or start over(S)? Your progress will be overwritten" << std::endl;
				std::string answer;
				std::cin >> answer;
				if(answer == "C" || answer == "c") {
					game(MemoryAboutLevelsProgress[tempLvl-1]);
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
					game(MemoryAboutLevelsProgress[tempLvl-1]);
				}
				else if(answer == "exit") {
					return gameProcessing("Begin", 0);
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
					game(MemoryAboutLevelsProgress[tempLvl-1]);
				}
				else {
					return gameProcessing("Begin", 0);
				}
			}
		}
		else {
			game(MemoryAboutLevelsProgress[tempLvl-1]);
		}
	}
	else if(point == "LevelComplete") {
		LevelProgress* completedLevel = MemoryAboutLevelsProgress[tempLvl-1];
		int correct = completedLevel->countOfCorrectProcessedRequests;
		int total = countOfReQuestsOnTheLevel[tempLvl-1];
		std::string answer;
		double percent = double(correct) / double(total);
		// если выполнено больше 75 процентов запросов и уровень не последний, то мы можем перейти на следующий уровень:
		if(percent >= 0.75) {
			if(tempLvl != countOfReQuestsOnTheLevel.size()) { // сравниваем с размером количества запросов на уровне, то есть с количеством уровней
				// эта часть если уровень не последний
				std::cout << "Nice work! You have " << correct << "/" << total << " passed requests. You can go to the next level(n) or improve your result(i)." << std::endl;
				std::cin >> answer;
				if(answer == "n" || answer == "N") {
					// Проверка прогресса на следующем уровне
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
							return gameProcessing("Begin", 0);
						}
					}
					game(MemoryAboutLevelsProgress[tempLvl]); // переходим на следующий уровень
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
					game(MemoryAboutLevelsProgress[tempLvl-1]); // улучшаем наш результат
				}
				else if(answer == "exit") {
					return gameProcessing("Begin", 0);
				}
			}
			else { // если последний уровень, то можно только улучшить результат
				if(correct == total) { 
					// если прошли на максимум, то выходим в главное меню на выбор уровня
					std::cout << "You have max result!" << std::endl;
					gameProcessing("Begin", 0);
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
						game(MemoryAboutLevelsProgress[tempLvl-1]);
					}
					else {
						return gameProcessing("Begin", 0);
					}
				}
			}
		}
		else { // нужно либо перепройти уровень либо выбрать другой ниже
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
				game(MemoryAboutLevelsProgress[tempLvl-1]);
			}
			else {
				return gameProcessing("Begin", 0);
			}
		}
	}
		
}