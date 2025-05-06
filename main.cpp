#include <iostream>
#include "Airplane.cpp"
#include "Airport.cpp"
#include "VPP.cpp"
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

int main() {

    Airport port;
	std::vector<int>lenghts(5, 10000);
	std::vector<int>types(6);
	types = { 0, 1, 2, 3, 4, 5 };
	port.set_vpps(lenghts);
    port.game(1);
	
	return 0;
}