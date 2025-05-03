#pragma once
#include<string>

class Airplane {
private:
	std::string type;
	int vpp_lenght;
	int time;
	int max_circles;

	std::string request;
	int circle = 0;

public:
	Airplane(std::string type_, std::string request_);

	void set_data(int vpp_lenght_, int time_, int max_circles_);

};