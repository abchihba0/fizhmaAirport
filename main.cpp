#include "Airplane.h"
#include"Airport.h"

int main() {
	Airport port;
	std::vector<int>lenghts(5, 10);
	std::vector<int>types(6);
	types = { 0, 1, 2, 3, 4, 5 };
	port.set_vpps(lenghts);
	port.set_manager(types);
	port.get_vpps();
	port.get_manager();
	return 0;
}