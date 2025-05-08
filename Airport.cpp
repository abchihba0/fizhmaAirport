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
            MemoryAboutLevelsProgress[level]->vpps = MemoryAboutLevelsProgress[level-1]->vpps;
            for (int i = MemoryAboutLevelsProgress[level-1]->vpps.size(); i < runwaysNeeded; ++i) {
                MemoryAboutLevelsProgress[level]->vpps.push_back(new VPP(lengths[i]));
            }
        }
    }

    // Инициализация текущих полос (для уровня 1 по умолчанию)
    vpps = MemoryAboutLevelsProgress[0]->vpps;
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

void Airport::game(int ourLevel)
{
	srand(time(0));
	double percentageOfCurrentLevelPassed = 0.0; // создаем переменную, в которую после обработки всех запросов запишется процент их прохождения
	int countOfPassedRequests = 0;
	for(int i = 0; i < countOfReQuestsOnTheLevel[ourLevel-1]; i++) {
		Airplane *tempPlane = set_manager();
		int result = processing(tempPlane);
		if(result == 1) { countOfPassedRequests+=1;}
		else {
			manager.push_back(tempPlane);
		}
		// обработка запроса
		// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // задержка между запросами в 5 секунд
	}	

}
int Airport::processing(Airplane *tempPlane)
{
	std::cout << "Type of request Plane: " << tempPlane->getType() << " Max Circle: "
	<< tempPlane->getMaxCircle() << " Need Length: " << tempPlane->getVppLength() << std::endl;
	std::cout << "busyness of vpp: " << std::endl;
	for(int i = 0; i < vpp_count; i++) {
		std::cout << "vpp number " << i+1 << ": " << "time busy: " << vpps[i]->getBusyTime() << ", length: " << vpps[i]->get_lenght() << std::endl;
	}
	int choice;
	std::cin >> choice; // 0 - принимать запрос, -1 - отправить на второй круг
	if(choice == 0) {
		int tempVpp;
		std::cin >> tempVpp;
		while(vpps[tempVpp-1]->getBusyTime() != 0) {
			std::cout << "This vpp is busy, Please, choose another vpp" << std::endl;
			std::cin >> tempVpp;
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
		std::cout << "Fuck you nigga" << std::endl;
		return 0;
	}
	
}

void Airport::gameProcessing()
{
	std::cout << "Which Level do you want to pass?" << std::endl;
	int tempLvl;
	std::cin >> tempLvl;
	if(tempLvl == 1) {
		if(MemoryAboutLevelsProgress[0]->countOfProcessedRequests > 0 &&
		MemoryAboutLevelsProgress[0]->countOfProcessedRequests < countOfReQuestsOnTheLevel[0] ) {
			std::cout << "You have already started this level before, but did not finish it. Do you want to continue(C) or start over(S)? Your progress will be overwritten" << std::endl;
			std::string answer;
			cin >> answer;
			if(answer == "C") {
				// нужно доработать функцию game, чтобы она начинала с сохраненного результата
			}
			else if(answer == "S") {
				game(1);
			}
		}
		else if(MemoryAboutLevelsProgress[0]->countOfProcessedRequests > 0 &&
		MemoryAboutLevelsProgress[0]->countOfProcessedRequests == countOfReQuestsOnTheLevel[0] ) {
			std::cout << "you have already completed this level before. Your result: "<< MemoryAboutLevelsProgress[0]->countOfCorrectProcessedRequests
			 << " correct queries from : " << countOfReQuestsOnTheLevel[0] << ". do you want to replay the level(Y/n)? Your current result will be overwritten." << std::endl;
		}
		std::string answer;
		cin >> answer;
		if(answer == "Y" || answer == "y") {
			game(1);
		}
		else if(answer == "N" || answer == "n"){
			gameProcessing();
		}
	}
	else {
		if(MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests > 0 &&
			MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests < countOfReQuestsOnTheLevel[tempLvl-1] ) {
				std::cout << "You have already started this level before, but did not finish it. Do you want to continue(C) or start over(S)? Your progress will be overwritten" << std::endl;
				std::string answer;
				cin >> answer;
				if(answer == "C") {
					// нужно доработать функцию game, чтобы она начинала с сохраненного результата
				}
				else if(answer == "S") {
					game(1);
				}
			}
			else if(MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests > 0 &&
			MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests == countOfReQuestsOnTheLevel[tempLvl-1] ) {
				std::cout << "you have already completed this level before. Your result: "<< MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests
				 << " correct queries from : " << countOfReQuestsOnTheLevel[tempLvl-1] << ". do you want to replay the level(Y/n)? Your current result will be overwritten." << std::endl;
			}
			std::string answer;
			cin >> answer;
			if(answer == "Y") {
				game(tempLvl-1);
			}
			else {
				gameProcessing();
			}
	}
}