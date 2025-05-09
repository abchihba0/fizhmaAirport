#pragma once
#include <vector>
#include "Airplane.h"
#include "VPP.h"
// количество уровней нужно продумать, оно должно быть нам известно, в примере ниже уровней допустим будет 5
// создаем структуру, которая будет хранить прогресс на уровне, то есть массив с занятостью vpps в данный момент, ОЧЕРЕДЬ manager из самолетов на следующих кругах 
struct LevelProgress {
	int Level = 1;
	std::vector<VPP*>vpps; // массив из впп
	std::vector<Airplane*>manager;
	int countOfProcessedRequests = 0; // количество обработанных запросов
	int countOfCorrectProcessedRequests = 0; // количество тех запросов, которые были обработанны корректно(то есть самолет был посажен или взлетел(те запросы, которые хранят самолеты на следующих кругах ни там ни там, они в доп массиве manager))
	int PercentageOfPassing = 0;

};

class Airport {
private:
	int vpp_count;
	unsigned int currentLevel = 1; // изначально уровень равен 1
	std::vector<VPP*>vpps; // массив из впп
	std::vector<Airplane*>manager; // в этот массив будут добавляться запросы 
	std::vector<int>countOfReQuestsOnTheLevel = {15, 20, 25, 30, 35}; // количество запросов на разных уровнях
	const std::vector<int> runwaysPerLevel = {3, 5, 7, 7, 7}; // Количество полос для каждого уровня
	std::vector<LevelProgress*>MemoryAboutLevelsProgress; // надо крч такой вектор чтобы можно было оттуда доставать прогресс уровня из структуры сразу, то есть по умолчанию все значения ноль и потом при выходе из игры прогрес перезаписывается
	std::vector<double> percentageOfLevelPassed = std::vector<double>(5, 0.0);  // создаем вектор, который будет хранить процент прохожения уровня. Если процент больше условно 75, то можно переходить к следующему, уровень пройден, однако всегда можно вернуться и перепройти 
public:

	Airport();

	int get_vpp_count()const { return vpp_count; }
	void get_vpps()const;	


	Airplane* set_manager();
	int processing(Airplane* tempPlane);
	void game(LevelProgress* ourLevel);
	void gameProcessing();
};
