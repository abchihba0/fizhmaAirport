#include <iostream>
#include "Airplane.cpp"
#include "Airport.cpp"
#include "VPP.cpp"
#include <cstdlib>
#include <ctime>
#include <vector>

int main() {

    Airport port;
	std::cout << "Hello=) This is a game that simulates the work of an airport manager. \nDuring the game you need to process requests coming to the control center.\nYou can allow the takeoff or landing of the aircraft, or you can send the aircraft on a second circle. \nAt each level there is a certain number of requests that you will receive, you need to process them all.\nTo exit to the main menu, you can write exit at any time the game is going on." << std::endl;
	std::string answer;
	// port.returnMemory()[0]->countOfProcessedRequests = 11;
	// port.returnMemory()[0]->countOfCorrectProcessedRequests = 11;
	std::cout << "Are you ready to go(Y/n)?" << std::endl;
	std::cin >> answer;
	if(answer == "Y" || answer == "y") {
		port.gameProcessing("Begin", 0); // begin отвечает за часть обработки уровня, перед или после. Нолик ни на что здесь не влияет, это тоже для окончания уровня

	}
	
	return 0;
}