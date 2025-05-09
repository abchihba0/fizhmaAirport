#include <iostream>
#include "Airplane.cpp"
#include "Airport.cpp"
#include "VPP.cpp"
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>
#include <thread>
#include <atomic>
#include <iomanip>
#include <mutex>
// std::mutex cout_mutex;

std::atomic<bool> keepRunning(true);

void timerThread() {
    //auto start = std::chrono::steady_clock::now();
	auto startTime = std::chrono::high_resolution_clock::now();
    while (keepRunning) {
        // auto now = std::chrono::steady_clock::now();
        // auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start);
        // int minutes = elapsed.count() / 60;
        // int seconds = elapsed.count() % 60;
		auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
        int acceleratedSeconds = elapsed*10/1000;
        int minutes = acceleratedSeconds/60;
        int seconds = acceleratedSeconds%60;
        
        // Блокируем вывод, чтобы избежать наложения
        std::unique_lock<std::mutex> lock(cout_mutex);
        // Сохраняем позицию курсора
        std::cout << "\033[s";
        // Перемещаемся в начало первой строки
        std::cout << "\033[1;1H";
        // Выводим таймер
        std::cout << "Time: " << std::setw(2) << std::setfill('0') << minutes 
                  << ":" << std::setw(2) << std::setfill('0') << seconds << " min:sec ";
        // Восстанавливаем позицию курсора
        std::cout << "\033[u";
        std::cout.flush();
        lock.unlock();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

int main() {
	// Полная очистка экрана и перемещение курсора в начало
    std::cout << "\033[2J\033[1;1H";
    
    std::thread timer(timerThread);

    Airport port;
	std::vector<int>lenghts(5, 10000);
	std::vector<int>types(6);
	types = { 0, 1, 2, 3, 4, 5 };
	port.set_vpps(lenghts);
    port.game(1);

	keepRunning = false;
    timer.join();

	//port.gameProcessing();
	
	return 0;
}