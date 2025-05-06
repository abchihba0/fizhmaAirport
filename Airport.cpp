#include "Airport.h"
#include<iostream>

VPP::VPP(int lenght_) :lenght(lenght_), status(false)
{
}

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

void Airport::get_manager() const
{
	for (int i = 0; i < plane_count; i++)
		std::cout << "Plane number " << i
		<< ": type " << manager[i]->getType()
		<< ", VPP " << manager[i]->getVppLength()
		<< std::endl;
}
