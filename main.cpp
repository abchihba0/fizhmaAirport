#include "Airplane.h"
#include"Airport.h"

int main() {
	Airport port;
	std::vector<int>lenghts(5, 5000);
	std::vector<int>types(6);
	types = { 4, 3, 1, 5, 2, 0 };
	port.set_vpps(lenghts);
	port.set_manager(types);
	port.get_vpps();
	port.get_manager();
	return 0;
}