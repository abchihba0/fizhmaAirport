#include <iostream>
#include "Airplane.cpp"
#include <cstdlib>
#include <ctime>
#include <vector>

using namespace std;

int main() {
    srand(time(0));


    vector<Airplane*> airplanes(10);
    for (int i = 0; i < 10; i++) {
        int airplaneType = rand() / (RAND_MAX / 6 + 1); 

        switch (airplaneType) {
            case 0: airplanes[i] = new CargoPlane(); break;
            case 1: airplanes[i] = new PassengerPlane(); break;
            case 2: airplanes[i] = new AgriculturePlane(); break;
            case 3: airplanes[i] = new MilitaryPlane(); break;
            case 4: airplanes[i] = new BusinessPlane(); break;
            case 5: airplanes[i] = new RescuePlane(); break;
        }

        std::cout << "Created: " << airplanes[i]->getType() << " (VPP: " << airplanes[i]->getVppLength() << "m)\n";
    }

    for (int i = 0; i < 10; i++) {
        delete airplanes[i];
    }
    // 	Airport port;
	std::vector<int>lenghts(5, 10);
	std::vector<int>types(6);
	types = { 0, 1, 2, 3, 4, 5 };
	port.set_vpps(lenghts);
	port.set_manager(types);
	port.get_vpps();
	port.get_manager();
	return 0;
}