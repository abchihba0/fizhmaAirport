#pragma once
#include <vector>
#include "Airplane.h"
#include "VPP.h"
#include <queue>
// количество уровней нужно продумать, оно должно быть нам известно, в примере ниже уровней допустим будет 5

// компаратор для очереди
struct CompareByTimeOnTheCircle {
	bool operator()(Airplane* a, Airplane* b) {
		return a->getTime() > b->getTime();
	}
};

// создаем структуру, которая будет хранить прогресс на уровне, то есть массив с занятостью vpps в данный момент, ОЧЕРЕДЬ manager из самолетов на следующих кругах 
struct LevelProgress {
	int Level = 1;
	int vpp_count;
	std::vector<VPP*>vpps; // массив из впп
	// std::vector<Airplane*> manager;// в этот массив будут добавляться запросы 
	std::priority_queue<Airplane*,std::vector<Airplane*>,CompareByTimeOnTheCircle> managerLanding;// в этот массив будут добавляться запросы на посадку
	std::priority_queue<Airplane*,std::vector<Airplane*>,CompareByTimeOnTheCircle> managerRise;// в этот массив будут добавляться запросы на взлёт
	int countOfProcessedRequests = 0; // количество обработанных запросов
	int countOfCorrectProcessedRequests = 0; // количество тех запросов, которые были обработанны корректно(то есть самолет был посажен или взлетел(те запросы, которые хранят самолеты на следующих кругах ни там ни там, они в доп массиве manager))
};


class Airport {
	private:std::vector<int>countOfReQuestsOnTheLevel = {15, 20, 25, 30, 35}; // количество запросов на разных уровнях
		const std::vector<int> runwaysPerLevel = {3, 5, 7, 7, 7}; // Количество полос для каждого уровня
		std::vector<LevelProgress*>MemoryAboutLevelsProgress; // надо крч такой вектор чтобы можно было оттуда доставать прогресс уровня из структуры сразу, то есть по умолчанию все значения ноль и потом при выходе из игры прогрес перезаписывается
	public:
		Airport();
		Airplane* set_manager(LevelProgress* ourLevel);
		int processing(Airplane* tempPlane, LevelProgress* ourLevel);
		void game(LevelProgress* ourLevel);
		void gameProcessing(std::string point,  int tempLvl);


		std::vector<LevelProgress*> returnMemory() {return MemoryAboutLevelsProgress;}
};