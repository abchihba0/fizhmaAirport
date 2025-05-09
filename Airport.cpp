#include "Airport.h"
#include<iostream>
#include <ctime>
#include <chrono>  // для времени
#include <thread>  // для sleep_for
#include <mutex>
std::mutex cout_mutex;


Airport::Airport(): vpp_count(0), plane_count(0),
vpps({}), manager({})
{
}

void Airport::set_vpps(std::vector<int> lengths_)
{
	vpp_count = lengths_.size();
	vpps = std::vector<VPP*>(vpp_count);
	for (int i = 0; i < vpp_count; i++)
		vpps[i] = new VPP(lengths_[i]);
}

void Airport::get_vpps() const
{
	std::unique_lock<std::mutex> lock(cout_mutex);
	std::cout << "\n";
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
		std::unique_lock<std::mutex> lock(cout_mutex);
        // Перемещаем курсор на вторую строку и очищаем все ниже
        std::cout << "\033[2;1H\033[J";
        lock.unlock();

		Airplane *tempPlane = set_manager();
		int result = processing(tempPlane);
		if(result == 1) { countOfPassedRequests+=1;}
		else {
			manager.push_back(tempPlane);
		}
		// обработка запроса
		// std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // задержка между запросами в 5 секунд
	}
	// Очистка manager при завершении уровня
    for (auto plane : manager) {
        delete plane;
    }
    manager.clear();	

}

int Airport::processing(Airplane *tempPlane)
{
	std::unique_lock<std::mutex> lock(cout_mutex);
    // Перемещаем курсор на вторую строку и очищаем все ниже
    std::cout << "\033[2;1H\033[J";

	std::cout << "Type of request Plane: " << tempPlane->getType() << " Max Circle: "
  	<< tempPlane->getMaxCircle() << " Need Length: " << tempPlane->getVppLength() << std::endl;
	std::cout << "busyness of vpp: " << std::endl;

	for(int i = 0; i < vpp_count; i++) {
		std::cout << "vpp number " << i+1 << " :" << "time busy: " << vpps[i]->getBusyTime() << ", length: " << vpps[i]->get_lenght() << std::endl;
	}
	lock.unlock();

	int choice;
	std::cin >> choice; // 0 - принимать запрос, -1 - отправить на второй круг
	if(choice == 0) {
		int tempVpp;
		std::cin >> tempVpp;
		while(vpps[tempVpp-1]->getBusyTime() != 0) {
			std::cout << "This vpp is busy. Please, choose another vpp" << std::endl;
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
		std::cout << "Wrong format" << std::endl;
		return 0;
	}
	
}

void Airport::gameProcessing()
{
	std::cout << "Which Level do you want to pass?" << std::endl;
	int tempLvl;
	std::cin >> tempLvl;
	// if(this.)
}