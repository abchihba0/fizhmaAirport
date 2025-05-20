#include "Airport.h"
#include<iostream>
#include<ctime>

Airport::Airport() :vpp_count(0), plane_count(0),
vpps({}), manager({})
{
}

void Airport::set_vpps(std::vector<int> lenghts_)
{
	vpp_count = lenghts_.size();
	vpps = std::vector<VPP*>(vpp_count);
	for (int i = 0; i < vpp_count; i++)
		vpps[i] = new VPP(lenghts_[i]);
}

void Airport::get_vpps() const
{
	for (int i = 0; i < vpp_count; i++)
		std::cout << "VPP " << i << ": lenght " <<
		vpps[i]->get_lenght() << ", status " <<
		(vpps[i]->get_status() ? "false" : "free") << std::endl;
}

void Airport::set_manager(std::vector<int> types_)
{
	plane_count = types_.size();
	manager = std::vector<Airplane*>(plane_count);

	for (int i = 0; i < plane_count; i++) {
		int airplaneType = types_[i];

		switch (airplaneType) {
		case 0: manager[i] = new CargoPlane(); break;
		case 1: manager[i] = new PassengerPlane(); break;
		case 2: manager[i] = new AgriculturePlane(); break;
		case 3: manager[i] = new MilitaryPlane(); break;
		case 4: manager[i] = new BusinessPlane(); break;
		case 5: manager[i] = new RescuePlane(); break;
		}
	}
}

Airplane* Airport::set_manager(){
	srand(time(0));

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
			if (vpp->get_lenght >= typesOfPlanesToGenerate[random_type]->getVppLength()) {//если находится нужная
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

void Airport::get_manager() const
{
	for (int i = 0; i < plane_count; i++)
		std::cout << "Plane number " << i
		<< ": type " << manager[i]->getType()
		<< ", VPP " << manager[i]->getVppLength()
		<< std::endl;
}
