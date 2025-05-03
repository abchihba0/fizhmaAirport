#include <iostream>
#include "Airplane.h"
#include "Airport.h"

int main() {
    Airport airport;
    std::vector<int>lenght(5, 10);
    airport.set_vpps(lenght);
    airport.get_vpps();
    return 0;
}