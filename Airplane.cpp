#include "Airplane.h"
#include <iostream>

Airplane::Airplane(std::string type_, std::string request_) :type(type_),
request(request_)//вся информация получена от ChatGPT ака Жора
{
	if (type == "Region")
		set_data(1500, 30, 3);//1500 м, 30 секунд, 3 круга
}

void Airplane::set_data(int vpp_lenght_, int time_, int max_circles_)
{
	vpp_lenght = vpp_lenght_;
	time = time_;
	max_circles = max_circles_;
}

