#pragma once
#include <vector>
#include "Airplane.h"
#include "VPP.h"
// количество уровней нужно продумать, оно должно быть нам известно, в примере ниже уровней допустим будет 5
class Airport {
private:
	int vpp_count;
	int plane_count;
	unsigned int currentLevel = 1; // изначально уровень равен 1
	std::vector<VPP*>vpps; // массив из впп
	std::vector<Airplane*>manager; // в этот массив будут добавляться запросы 
	std::vector<int>countOfReQuestsOnTheLevel = {15, 20, 25, 30, 35}; // количество запросов на разных уровнях
	std::vector<double> percentageOfLevelPassed = std::vector<double>(5, 0.0);  // создаем вектор, который будет хранить процент прохожения уровня. Если процент больше условно 75, то можно переходить к следующему, уровень пройден, однако всегда можно вернуться и перепройти 
public:

	Airport();

	void set_vpp_count(int count_) { vpp_count = count_; }
	void set_plane_count(int count_) { plane_count = count_; }
	void set_vpps(std::vector<int>lenghts_);

	int get_vpp_count()const { return vpp_count; }
	int get_plane_count()const { return plane_count; }
	void get_vpps()const;	


	Airplane* set_manager();
	int processing(Airplane* tempPlane);
	void game(int ourLevel);
};
//