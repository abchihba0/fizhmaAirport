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
    for (int i = 0; i < countOfReQuestsOnTheLevel.size(); ++i) {
        MemoryAboutLevelsProgress.push_back(new LevelProgress());
		MemoryAboutLevelsProgress[i]->Level = (i+1); // забиваем лвл в переменную каждому представителю LevelProgress
		MemoryAboutLevelsProgress[i]->vpp_count = runwaysPerLevel[i]; // забиваем колво полос в переменную каждому представителю LevelProgress
    }

    // Генерация полос для каждого уровня
    for (int level = 0; level <  countOfReQuestsOnTheLevel.size(); ++level) {
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
				if(vpp->getBusyTime() < typesOfPlanesToGenerate[random_type]->getTime()*typesOfPlanesToGenerate[random_type]->getMaxCircle()) {
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
	srand(time(0));
	std::cout << "You have " << ourLevel->vpp_count << " vpps on " << ourLevel->Level << " level" << std::endl;
	double percentageOfCurrentLevelPassed = 0.0; // создаем переменную, в которую после обработки всех запросов запишется процент их прохождения
	// колво правильных запросов равно количеству корректных запросов их сохраненного результата
	for(int i = 0; i < countOfReQuestsOnTheLevel[ourLevel->Level-1] - ourLevel->countOfProcessedRequests; i++) { // количество запросов которые нужно обработать = количество запросов на уровне - количество уже обработанных запросов
		Airplane *tempPlane = set_manager(ourLevel);
		int result = processing(tempPlane, ourLevel);
		if(result == 1) { ourLevel->countOfCorrectProcessedRequests+=1;}
		else if(result == -1) {
			ourLevel->manager.push_back(tempPlane);
		}
		// обработка запроса
		// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // задержка между запросами в 5 секунд
	}
	
	// Перенаправляем в gameProcessing для обработки завершения уровня
	gameProcessing("LevelComplete", ourLevel->Level);
}


int Airport::processing(Airplane *tempPlane, LevelProgress* ourLevel)
{
	std::cout << "Type of request Plane: " << tempPlane->getType() << " Max Circle: "
	<< tempPlane->getMaxCircle() << " Need Length: " << tempPlane->getVppLength() << std::endl;
	std::cout << "busyness of vpp: " << std::endl;
	for(int i = 0; i < ourLevel->vpp_count; i++) {
		std::cout << "vpp number " << i+1 << ": " << "time busy: " << ourLevel->vpps[i]->getBusyTime() << ", length: " << ourLevel->vpps[i]->get_lenght() << std::endl;
	}
	int choice = 5;
	while (choice != 0 || choice != -1 || choice != 2)
	{
		std::cin >> choice; // 0 - принимать запрос, -1 - отправить на второй круг

		if(choice == 0) {
			int tempVpp;
			std::cin >> tempVpp;
			while(ourLevel->vpps[tempVpp-1]->getBusyTime() != 0) {
				std::cout << "This vpp is busy. Please, choose another vpp" << std::endl;
				std::cin >> tempVpp;
			}
			// тут должна быть привязка ко времени
			ourLevel->vpps[tempVpp-1]->setBusyTime(tempPlane->getTime());
			return 1;
		}
		else if(choice == -1) {
			// пока что они не прилетают со второго круга=)
			return -1;
		}
		else {
			std::cout << "Wrong format" << std::endl;
			continue;
		}	
	}
}


void Airport::gameProcessing(std::string point, int tempLvl)
{	
    if(point == "Begin") {
        std::cout << "To accept the request, you need to press 0, then there will be a choice between different VPPS. \nTo send the plane on a next circle or ask it to wait for takeoff, you need to press -1." << std::endl;
        
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
        std::cin >> tempLvl;

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
                if(answer == "C") {
                    game(MemoryAboutLevelsProgress[tempLvl-1]);
                }
                else if(answer == "S") {
					for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
						MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
					}
					MemoryAboutLevelsProgress[tempLvl-1]->manager.resize(0);
					MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
					MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
                    game(MemoryAboutLevelsProgress[tempLvl-1]);
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
					MemoryAboutLevelsProgress[tempLvl-1]->manager.resize(0);
					MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
					MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
                    game(MemoryAboutLevelsProgress[tempLvl-1]);
                }
                else {
                    gameProcessing("Begin", 0);
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
        // если выполнено больше 75 процентов запросов и уровень не последний, то мы можем перейти на следующий уровень:
        if(correct > total/4*3) {
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
                        std::string overwriteAnswer;
                        std::cin >> overwriteAnswer;
                        if(overwriteAnswer == "Y" || overwriteAnswer == "y") {
                            for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
								MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
							}
							MemoryAboutLevelsProgress[tempLvl-1]->manager.resize(0);
							MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
							MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
                        }
                    }
                    game(MemoryAboutLevelsProgress[tempLvl]); // переходим на следующий уровень
                }
                else if(answer == "I" || answer == "i") {
                    for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
						MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
					}
					MemoryAboutLevelsProgress[tempLvl-1]->manager.resize(0);
					MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
					MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
                    game(MemoryAboutLevelsProgress[tempLvl-1]); // улучшаем наш результат
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
						MemoryAboutLevelsProgress[tempLvl-1]->manager.resize(0);
						MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
						MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
                        game(MemoryAboutLevelsProgress[tempLvl-1]);
                    }
                    else {
                        gameProcessing("Begin", 0);
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
				MemoryAboutLevelsProgress[tempLvl-1]->manager.resize(0);
				MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
				MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
                game(MemoryAboutLevelsProgress[tempLvl-1]);
            }
            else {
                gameProcessing("Begin", 0);
            }
        }
    }
}