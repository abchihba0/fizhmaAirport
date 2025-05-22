#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "Airport.h"
#include<iostream>
#include <ctime>
#include <chrono>  // для времени
#include <thread>  // для sleep_for
#include <algorithm>
#include <queue>
#include <sstream>
#include <string>
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

void Airport::game(LevelProgress* ourLevel, GLFWwindow* window)
{
	srand(time(0));
	ImGui::Text("You have %d vpps on %d level", ourLevel->vpp_count, ourLevel->Level);
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
				ImGui::Text("We have %d planes who flew in from the circle", PlanesWhoFlewInFromTheCircle.size());
				ImGui::Text("you must send it to the second round or accept it");
				for(int i = 0; i < PlanesWhoFlewInFromTheCircle.size(); i++) {
					result = processing(PlanesWhoFlewInFromTheCircle[i], ourLevel);
					if(result == 2) { 
						ImGui::Text("Progress saved. Returning to level selection...");
						return gameProcessing("Begin", ourLevel->Level, window);;
					}
					else if(result == 1) { 
						ourLevel->countOfCorrectProcessedRequests+=1;
						ourLevel->countOfProcessedRequests+=1;
					}
					else if(result == -1) { 
						if (PlanesWhoFlewInFromTheCircle[i]->getCircle() == PlanesWhoFlewInFromTheCircle[i]->getMaxCircle()) {
							ImGui::Text("It was the last circle, the request was skipped.");
						}
						else {
							PlanesWhoFlewInFromTheCircle[i]->increaseCircle();
							PlanesWhoFlewInFromTheCircle[i]->setTime(PlanesWhoFlewInFromTheCircle[i]->getRequiredTime());
							ourLevel->managerLanding.push(PlanesWhoFlewInFromTheCircle[i]);
						}
					}
					else if(result == -2) { 
						if (PlanesWhoWantToFly[i]->getCircle() == PlanesWhoWantToFly[i]->getMaxCircle()) {
							ImGui::Text("It was the last circle, the request was skipped.");
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
					ImGui::Text("Progress saved. Returning to level selection...");
					return gameProcessing("Begin", ourLevel->Level, window);
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
				ImGui::Text("We have %d planes that want to take off", PlanesWhoWantToFly.size());
				ImGui::Text("you must send it to the second round or accept it");
				for(int i = 0; i < PlanesWhoWantToFly.size(); i++) {
					result = processing(PlanesWhoWantToFly[i], ourLevel);
					if(result == 2) { 
						ImGui::Text("Progress saved. Returning to level selection...");
						return gameProcessing("Begin", ourLevel->Level, window);
					}
					else if(result == 1) { 
						ourLevel->countOfCorrectProcessedRequests+=1;
						ourLevel->countOfProcessedRequests+=1;
					}
					else if(result == -1) { 
						if (PlanesWhoFlewInFromTheCircle[i]->getCircle() == PlanesWhoFlewInFromTheCircle[i]->getMaxCircle()) {
							ImGui::Text("It was the last circle, the request was skipped.");
						}
						else {
							PlanesWhoFlewInFromTheCircle[i]->increaseCircle();
							PlanesWhoFlewInFromTheCircle[i]->setTime(PlanesWhoFlewInFromTheCircle[i]->getRequiredTime());
							ourLevel->managerLanding.push(PlanesWhoFlewInFromTheCircle[i]);
						}
					}
					else if(result == -2) { 
						if (PlanesWhoWantToFly[i]->getCircle() == PlanesWhoWantToFly[i]->getMaxCircle()) {
							ImGui::Text("It was the last circle, the request was skipped.");
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
					ImGui::Text("Progress saved. Returning to level selection...");
					return gameProcessing("Begin", ourLevel->Level, window);;
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
				ImGui::Text("Progress saved. Returning to level selection...");
				return gameProcessing("Begin", ourLevel->Level, window);
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
	
	// Перенаправляем в gameProcessing для обработки завершения уровня
	gameProcessing("LevelComplete", ourLevel->Level, window);
}


int Airport::processing(Airplane *tempPlane, LevelProgress* ourLevel)
{
    
    int typeOfRequest = rand() % 2;
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "Current Request:");
    ImGui::Spacing();
    ImGui::Text("Type of request Plane: %s", tempPlane->getType());
    ImGui::Text("Request type: %s", ((typeOfRequest==0) ? "landing " : "takeoff "));
    ImGui::Text("Max Circle: %d", tempPlane->getMaxCircle());
    ImGui::Text("Need Length: %d", tempPlane->getVppLength());
    ImGui::Text("Time for next circle: %d", tempPlane->getRequiredTime());
    ImGui::Text("Current Circle: %d", tempPlane->getCircle());
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "busyness of vpp:");
    for(int i = 0; i < ourLevel->vpp_count; i++) {
        ImGui::Text("vpp number %d:  time busy: %d, length: %d", i+1, ourLevel->vpps[i]->getBusyTime(), ourLevel->vpps[i]->get_lenght());
    }
    for (int i = 0; i < ourLevel->vpp_count; ++i) {
        if (i > 0) ImGui::SameLine();
        bool is_busy = ourLevel->vpps[i]->getBusyTime() > 0;
        bool too_short = tempPlane && (ourLevel->vpps[i]->get_lenght() < tempPlane->getVppLength());
        
        if (is_busy || too_short) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.2f, 0.2f, 1.0f));
        }

        if (ImGui::Button(("Runway " + std::to_string(i+1)).c_str(), ImVec2(100, 40))) {
            if (!is_busy && !too_short) {
                ImGui::Text("Selected Runway %d", i+1);
                ourLevel->vpps[i]->setBusyTime(tempPlane->getRequiredTime());
                if (is_busy || too_short) {
                    ImGui::PopStyleColor(2);
                }
                return 1;
            }
        }

        if (is_busy || too_short) {
            ImGui::PopStyleColor(2);
        }
    }
    // Action buttons
	// tung sahur
	bool ispress = false;
	while(!ispress) {
		ImGui::Spacing();
		if (ImGui::Button("Send to Next Circle", ImVec2(230, 40)) && typeOfRequest == 0) {
			ImGui::Text("Sent to next circle");
			return -1;
		}
		// ImGui::SameLine();
		else if (ImGui::Button("Send to Next Circle", ImVec2(230, 40)) && typeOfRequest == 1) {
			ImGui::Text("Sent to next circle");
			return -2;
		}
		// ImGui::SameLine();
		else if (ImGui::Button("Skip Request", ImVec2(230, 40))) {
			ImGui::Text("Skipped request");
			return 0;
		}
		// ImGui::SameLine();
		else if (ImGui::Button("Exit to Menu", ImVec2(230, 40))) {
			return 2;
		}
		else {
			ispress = true;
		}
	}
	return 0;
}

void Airport::gameProcessing(std::string point, int tempLvl, GLFWwindow* window)
{	
	if(point == "Begin") {
		// Вывод всех доступных уровней с прогрессом
		ImGui::TextColored(ImVec4(0.0f, 0.8f, 1.0f, 1.0f), "Available levels:");
		for(int i = 0; i < countOfReQuestsOnTheLevel.size(); ++i) {
			if (i >= MemoryAboutLevelsProgress.size()) break;  // Safety check
			bool level_locked = (i > 0) && ((double)MemoryAboutLevelsProgress[i-1]->countOfCorrectProcessedRequests / countOfReQuestsOnTheLevel[i-1] < 0.75);
		
			if (level_locked) {
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
				ImGui::Text("Level %d: Locked (complete 75%% of level %d)", i+1, i);
				ImGui::PopStyleColor();
			} 
			else {
				ImGui::Spacing();
				if (ImGui::Button(("Level " + std::to_string(i+1)).c_str(), ImVec2(100, 40))) {
					// Проверяем, что индекс уровня корректен
                    if (i >= 0 && i < MemoryAboutLevelsProgress.size()) {
                        for(int j = 0; j < MemoryAboutLevelsProgress[i]->vpp_count; j++) {
                            MemoryAboutLevelsProgress[i]->vpps[j]->setBusyTime(0);
                        }
                        while(!MemoryAboutLevelsProgress[i]->managerLanding.empty()) {
                            MemoryAboutLevelsProgress[i]->managerLanding.pop();
                        }
                        MemoryAboutLevelsProgress[i]->countOfCorrectProcessedRequests = 0;
                        MemoryAboutLevelsProgress[i]->countOfProcessedRequests = 0;
                        game(MemoryAboutLevelsProgress[i], window);
                    }
					// for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
					// 	MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
					// }
					// while(!MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.empty()) {
					// 	MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.pop();
					// }
					// MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
					// MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
					// game(MemoryAboutLevelsProgress[tempLvl-1], window);
				}
			
				ImGui::SameLine();
				if (MemoryAboutLevelsProgress[i]->countOfProcessedRequests > 0) {
					double prevProgress = (double)MemoryAboutLevelsProgress[i]->countOfCorrectProcessedRequests / 
										countOfReQuestsOnTheLevel[i];
					ImGui::Text("Progress: %.1f%%", prevProgress * 100);
					ImGui::SameLine();
					ImGui::ProgressBar(prevProgress, ImVec2(200, 20));
				} else {
					ImGui::Text("Not started");
				}
			}
		}
		ImGui::Spacing();
		if (ImGui::Button("Exit", ImVec2(100, 40))) {
			return;
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
				ImGui::TextWrapped("Nice work! You have %d / %d passed requests. You can go to the next level or improve your result.", correct, total);
				ImGui::Spacing();
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
				if (ImGui::Button("Next level", ImVec2(200, 40))) {
					if(MemoryAboutLevelsProgress[tempLvl]->countOfProcessedRequests > 0) {
						ImGui::TextWrapped("Next level already has progress: %d / %d. Are you sure you want to overwrite it?", MemoryAboutLevelsProgress[tempLvl]->countOfCorrectProcessedRequests, countOfReQuestsOnTheLevel[tempLvl]);
						ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
						if (ImGui::Button("Yes", ImVec2(200, 40))) {
							for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
								MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
							}
							while(!MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.empty()) {
								MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.pop();
							}
							MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
							MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
						}
						ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
						if (ImGui::Button("No", ImVec2(200, 40))) {
							return gameProcessing("Begin", 0, window);
						}
					}
					game(MemoryAboutLevelsProgress[tempLvl], window); // переходим на следующий уровень
				}
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
				if (ImGui::Button("Improve result", ImVec2(200, 40))) {
					for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
						MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
					}
					while(!MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.empty()) {
						MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.pop();
					}
					MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
					MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
					game(MemoryAboutLevelsProgress[tempLvl-1], window); // улучшаем наш результат
				}
				ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
				if (ImGui::Button("Exit", ImVec2(200, 40))) {
					return gameProcessing("Begin", 0, window);
				}
			}
			else { // если последний уровень, то можно только улучшить результат
				if(correct == total) { 
					// если прошли на максимум, то выходим в главное меню на выбор уровня
					ImGui::Text("You have max result!");
					gameProcessing("Begin", 0, window);
				}
				else {
					ImGui::TextWrapped("Nice work! You have %d / %d passed requests. You can improve the result. Will you go?", correct, total);
					ImGui::Spacing();
					ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
					if (ImGui::Button("Yes", ImVec2(200, 40))) {
						for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
							MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
						}
						while(!MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.empty()) {
							MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.pop();
						}
						MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
						MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
						game(MemoryAboutLevelsProgress[tempLvl-1], window);
					}
					ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
					if (ImGui::Button("No", ImVec2(200, 40))) {
						return gameProcessing("Begin", 0, window);
					}
				}
			}
		}
		else { // нужно либо перепройти уровень либо выбрать другой ниже
			ImGui::TextWrapped("You need to replay this level or choose another. Replay?");
			ImGui::Spacing();
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
			if (ImGui::Button("Yes", ImVec2(200, 40))) {
				for(int i = 0; i < MemoryAboutLevelsProgress[tempLvl-1]->vpp_count; i++) {
					MemoryAboutLevelsProgress[tempLvl-1]->vpps[i]->setBusyTime(0);
				}
				while(!MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.empty()) {
					MemoryAboutLevelsProgress[tempLvl-1]->managerLanding.pop();
				}
				MemoryAboutLevelsProgress[tempLvl-1]->countOfCorrectProcessedRequests = 0;
				MemoryAboutLevelsProgress[tempLvl-1]->countOfProcessedRequests = 0;
				game(MemoryAboutLevelsProgress[tempLvl-1], window);
			}
			ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 200) * 0.5f);
			if (ImGui::Button("No", ImVec2(200, 40))) {
				return gameProcessing("Begin", 0, window);
			}
		}
	}
}


// std::string Airport::processCommand(const std::string& input, int level) {
//     std::stringstream output;
//     // Сохраняем старый буфер cout
//     auto old_buf = std::cout.rdbuf(output.rdbuf());
    
//     // Вызываем основную логику обработки
//     gameProcessing(input, level, window);
    
//     // Восстанавливаем cout
//     std::cout.rdbuf(old_buf);
    
//     return output.str();
// }
// int Airport::getLevelRequestCount(int level) const {
//     if (level >= 0 && level < countOfReQuestsOnTheLevel.size()) {
//         return countOfReQuestsOnTheLevel[level];
//     }
//     return 0;
// }