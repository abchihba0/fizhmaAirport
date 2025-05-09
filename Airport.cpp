#include "Airport.h"
#include<iostream>
#include <ctime>
#include <chrono>  // для времени
#include <thread>  // для sleep_for
#include <algorithm>



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
    for (int i = 0; i < 5; ++i) {
        MemoryAboutLevelsProgress.push_back(new LevelProgress());
		MemoryAboutLevelsProgress[i]->Level = (i+1); // забиваем лвл в переменную каждому представителю LevelProgress
    }

    // Генерация полос для каждого уровня
    for (int level = 0; level < 5; ++level) {
        int runwaysNeeded = runwaysPerLevel[level];
        std::vector<int> lengths;
        std::vector<int> remainingLengths = requiredLengths; // Копируем все требуемые длины

        // Генерация полос для текущего уровня
        for (int i = 0; i < runwaysNeeded; ++i) {
            bool canGenerateRunway = false;
            int randomLength;

            do {
                canGenerateRunway = false;
                // Если остались неприкрытые типы самолётов, выбираем длину из оставшихся
                if (!remainingLengths.empty()) {
                    int randomIndex = rand() % remainingLengths.size();
                    randomLength = remainingLengths[randomIndex] + (rand() % 5) * 100; // Добавляем случайный разброс
                } else {
                    // Все типы уже покрыты, генерируем любую допустимую длину
                    int minLength = *std::min_element(requiredLengths.begin(), requiredLengths.end());
                    randomLength = minLength + (rand() % 10) * 100;
                }

                // Проверяем, что хотя бы один тип самолёта можно посадить на эту полосу
                for (int len : requiredLengths) {
                    if (randomLength >= len) {
                        canGenerateRunway = true;
                        break;
                    }
                }

                if (canGenerateRunway) {
                    // Удаляем из remainingLengths длины, которые теперь покрыты этой полосой
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

        // Если это не первый уровень, копируем старые полосы и добавляем новые
        if (level > 0) {
            for (VPP* vpp : MemoryAboutLevelsProgress[level-1]->vpps) {
                VPP* newVpp = new VPP(vpp->get_lenght());
                MemoryAboutLevelsProgress[level]->vpps.push_back(newVpp);
            }
        }
    }

    // Инициализация текущих полос (для уровня 1 по умолчанию)
    // Инициализация текущих полос
    for (VPP* vpp : MemoryAboutLevelsProgress[0]->vpps) {
        vpps.push_back(new VPP(vpp->get_lenght()));
    }
    vpp_count = vpps.size();
}



void Airport::get_vpps() const
{
	for (int i = 0; i < vpp_count; i++)
		std::cout << "VPP " << i << ": lenght " <<
		vpps[i]->get_lenght() << ", status " <<
		(vpps[i]->get_status() ? "false" : "free") << std::endl;
}

Airplane* Airport::set_manager(){
  
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
  
	  for(auto vpp:vpps)//бегаем по дорожкам
		if (vpp->get_lenght() >= typesOfPlanesToGenerate[random_type]->getVppLength()) {//если находится нужная
		  can_we_generate_this_type = true;
		  break;
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

void Airport::saveProgress(LevelProgress* level) {
    level->countOfProcessedRequests = countOfReQuestsOnTheLevel[level->Level-1] - manager.size();
    level->countOfCorrectProcessedRequests = 0;
    level->vpps = vpps;
    level->manager = manager;
}

// void Airport::handleExit() {
//     if (currentLevelProgress != nullptr) {
//         saveProgress(currentLevelProgress);
//     }
//     shouldExit = true;
// }

// bool Airport::checkForExitCommand(int input) {
//     if (input == 2) {
//         handleExit();
//         return true;
//     }
//     return false;
// }

void Airport::game(LevelProgress* ourLevel)
{
	srand(time(0));
	vpps = ourLevel->vpps;
	vpp_count = ourLevel->vpps.size();
	std::cout << "You have " << vpps.size() << " vpps on " << ourLevel->Level << " level" << std::endl;
	double percentageOfCurrentLevelPassed = 0.0; // создаем переменную, в которую после обработки всех запросов запишется процент их прохождения
	int countOfPassedRequests = 0;
	for(int i = 0; i < countOfReQuestsOnTheLevel[ourLevel->Level-1] - ourLevel->countOfProcessedRequests; i++) { // количество запросов которые нужно обработать = количество запросов на уровне - количество уже обработанных запросов
		Airplane *tempPlane = set_manager();
		int result = processing(tempPlane);
		if(result == 2) { 
            saveProgress(ourLevel);
            std::cout << "Progress saved. Returning to level selection..." << std::endl;
            return;
        }
		else if(result == 1) { countOfPassedRequests+=1;}
		else if(result == -1) {
			manager.push_back(tempPlane);
		}
		// обработка запроса
		// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // задержка между запросами в 5 секунд
	}	
	saveProgress(ourLevel);
}


int Airport::processing(Airplane *tempPlane)
{
	std::cout << "Type of request Plane: " << tempPlane->getType() << " Max Circle: "
	<< tempPlane->getMaxCircle() << " Need Length: " << tempPlane->getVppLength() << std::endl;
	std::cout << "busyness of vpp: " << std::endl;
	for(int i = 0; i < vpp_count; i++) {
		std::cout << "vpp number " << i+1 << ": " << "time busy: " << vpps[i]->getBusyTime() << ", length: " << vpps[i]->get_lenght() << std::endl;
	}
	std::string input;
	std::cin >> input; // 0 - принимать запрос, -1 - отправить на второй круг
	if (input == "exit") {
        return 2; 
    }
	int choice=std::stoi(input);
	if(choice == 0) {
		std::string vppInput;
		std::cin >> vppInput;
		if (vppInput == "exit") {
			return 2;
		}
		int tempVpp = std::stoi(vppInput);
		while(vpps[tempVpp-1]->getBusyTime() != 0) {
			std::cout << "This vpp is busy, Please, choose another vpp" << std::endl;
			std::string vppInput;
			std::cin >> vppInput;
			if (vppInput == "exit") {
				return 2;
			}
			int tempVpp = std::stoi(vppInput);
		}
		// тут должна быть привязка ко времени
		vpps[tempVpp-1]->setBusyTime(tempPlane->getTime());
		return 1;
	}
	else if(choice == -1) {
		// пока что они не прилетают со второго круга=)
		return -1;
	}
	else {
		std::cout << "Wrong format" << std::endl;
		return 0;
	}
	
}

void Airport::gameProcessing()
{
	while (true)
	{
		std::cout << "To accept the request, you need to press 0, then there will be a choice between different VPPS. \nTo send the plane on a next circle or ask it to wait for takeoff, you need to press -1." << std::endl;
		std::cout << "Which Level do you want to pass?" << std::endl;
		std::string input;
		std::cin >> input;
		if(input == "exit") {
			std::cout << "Exiting game..." << std::endl;
			return;
		}
		int tempLvl=std::stoi(input);
		if(tempLvl == 1) {
			if(MemoryAboutLevelsProgress[0]->countOfProcessedRequests > 0) {
				if(MemoryAboutLevelsProgress[0]->countOfProcessedRequests < countOfReQuestsOnTheLevel[0] ) {
					std::cout << "You have already started this level before, but did not finish it. Do you want to continue(C) or start over(S)? Your progress will be overwritten" << std::endl;
					std::string answer;
					std::cin >> answer;
					if(answer == "C") {
						game(MemoryAboutLevelsProgress[0]);
					}
					else if(answer == "S") {
						MemoryAboutLevelsProgress[tempLvl-1] = new LevelProgress();
						MemoryAboutLevelsProgress[tempLvl-1]->Level = tempLvl;
						game(MemoryAboutLevelsProgress[0]);
					}
					else if(answer == "exit") {
						continue;
					}
				}
				else if(MemoryAboutLevelsProgress[0]->countOfProcessedRequests == countOfReQuestsOnTheLevel[0] ) {
					std::cout << "you have already completed this level before. Your result: "<< MemoryAboutLevelsProgress[0]->countOfCorrectProcessedRequests
						<< " correct queries from : " << countOfReQuestsOnTheLevel[0] << ". do you want to replay the level(Y/n)? Your current result will be overwritten." << std::endl;
				}
				std::string answer;
				std::cin >> answer;
				if(answer == "Y" || answer == "y") {
					MemoryAboutLevelsProgress[tempLvl-1] = new LevelProgress();
					MemoryAboutLevelsProgress[tempLvl-1]->Level = tempLvl;
					game(MemoryAboutLevelsProgress[tempLvl-1]);
				}
				else if(answer == "N" || answer == "n"){
					gameProcessing();
				}
				else if(answer == "exit") {
					continue;
				}
			}
			else {
				game(MemoryAboutLevelsProgress[0]);
			}
		}
		else {
			if(MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests > 0) {
				if(MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests < countOfReQuestsOnTheLevel[tempLvl-1] ) {
					std::cout << "You have already started this level before, but did not finish it. Do you want to continue(C) or start over(S)? Your progress will be overwritten" << std::endl;
					std::string answer;
					std::cin >> answer;
					if(answer == "C") {
						game(MemoryAboutLevelsProgress[tempLvl-1]);
					}
					else if(answer == "S") {
						MemoryAboutLevelsProgress[tempLvl-1] = new LevelProgress();
						MemoryAboutLevelsProgress[tempLvl-1]->Level = tempLvl;
						game(MemoryAboutLevelsProgress[tempLvl-1]);
					}
					else if(answer == "exit") {
						continue;
					}
				}
				else if(MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests == countOfReQuestsOnTheLevel[tempLvl-1] ) {
					std::cout << "you have already completed this level before. Your result: "<< MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests
						<< " correct queries from : " << countOfReQuestsOnTheLevel[tempLvl-1] << ". do you want to replay the level(Y/n)? Your current result will be overwritten." << std::endl;
				}
				std::string answer;
				std::cin >> answer;
				if(answer == "Y") {
					MemoryAboutLevelsProgress[tempLvl-1] = new LevelProgress();
					MemoryAboutLevelsProgress[tempLvl-1]->Level = tempLvl;
					game(MemoryAboutLevelsProgress[tempLvl-1]);
				}
				else if(answer == "exit") {
					continue;
				}
				else {
					gameProcessing();
				}
			}
			else {
				game(MemoryAboutLevelsProgress[tempLvl-1]);
			}
		}
	}	
	
}