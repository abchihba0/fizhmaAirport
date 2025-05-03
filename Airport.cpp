#include "Airport.h"
#include <iostream>

VPP::VPP(int lenght_) :lenght(lenght_), status(false)
{
}

Airport::Airport():count_airplane(0),count_vpp(0),vpps(nullptr),manager(nullptr)
{
}

void Airport::set_vpps(std::vector<int> lenght_)
{
	for (int i = 0; i < lenght_.size(); i++)
		vpps.push_back(VPP(lenght_[i]));
}

void Airport::get_vpps() const
{
	for (int i = 0; i < get_count_vpp(); i++)
		std::cout << "VPP " << i << ": lenght " << vpps[i].get_lenght 
		<< ", status " << (vpps[i].get_status()) ? "busy" : "free" << std::endl;
}
