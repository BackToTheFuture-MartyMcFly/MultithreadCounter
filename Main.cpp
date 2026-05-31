#include "MultithreadCounter.h"


void goSingle() {
	BackgroundCounter counter;
	counter.start();

	std::cout << "Commands:\np - pause the work\nr - resume the work\ns - stop work\nv - print the value\nq - exit" << std::endl;

	char cmd;
	while (std::cin >> cmd) {
		switch (cmd) {
		case 'p':
			counter.pause();
			break;
		case 'r':
			counter.resume();
			break;
		case 's':
			counter.stop();
			break;
		case 'v':
			std::cout << "Value: " << counter.getValue() << "\n";
			break;
		case 'q':
			counter.stop();
			return;
		default:
			std::cout << "Unknown command\n";
		}
	}
}

std::optional<int> tryToInt(const std::string& str) {
	try {
		size_t pos;
		int value = std::stoi(str, &pos);
		return (pos == str.length()) ? std::optional{ value } : std::nullopt;
	}
	catch (...) {
		return std::nullopt;
	}
}

void goMultiply() {
	CounterManager manager;

	// Создаём несколько счётчиков
	manager.createCounter(1, 5);      // счётчик 1 до 5
	manager.createCounter(2, 3);      // счётчик 2 до 3
	manager.createCounter(3, 10);     // счётчик 3 до 10

	// Запускаем все счётчики
	manager.startCounter(1);
	manager.startCounter(2);
	manager.startCounter(3);

	// Даём поработать 3 секунды
	std::this_thread::sleep_for(std::chrono::seconds(3));
	std::cout << "\n--- After 3 seconds ---\n";
	manager.printStatusCounters();

	// Ставим счётчик 1 на паузу на 4 секунды
	std::cout << "\n--- Pause the counter 1 from 4 seconds ---\n";
	manager.pauseCounter(1);
	std::this_thread::sleep_for(std::chrono::seconds(4));
	manager.resumeCounter(1);

	// Даём поработать ещё 2 секунды
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::cout << "\n--- After pause and resume ---\n";
	manager.printStatusCounters();

	// Останавливаем счётчик 2
	std::cout << "\n--- Stopping counter 2 ---\n";
	manager.stopCounter(2);

	// Даём поработать ещё 3 секунды
	std::this_thread::sleep_for(std::chrono::seconds(3));
	std::cout << "\n--- Final status ---\n";
	manager.printStatusCounters();

	// Останавливаем все счётчики
	std::cout << "\n--- Stopping all counters ---\n";
	manager.stopAllCounters();

	// Показываем, что менеджер пуст
	manager.printStatusCounters();

	std::cout << "\nProgramm done.\n";	
}
	
int main() {
	
	std::cout << "Choose the mode: single - s or multiply - m" << std::endl;
	char mode;
	while (std::cin >> mode) {
		switch (mode) {
		case 's':
			goSingle();
			return 0;
		case 'm':
			goMultiply();
			return 0;
		default:
			std::cout << "Enter the coorect command" << std::endl;
			break;
		}
	}

	return 0;
}
