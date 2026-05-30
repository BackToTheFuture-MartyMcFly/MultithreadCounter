
#include <variant>
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
		if (pos == str.length()) {
			return value;
		}
	}
	catch (const std::invalid_argument&) { }
	catch (const std::out_of_range&) {}
	return std::nullopt;
}

using Variant = std::variant<std::string, int>;

std::optional<std::vector<Variant>> parceCmd(std::string_view sv) {
	if (sv.empty())
		return std::nullopt;
	std::vector<Variant> result;
	auto pos1 = sv.find_first_of(' ');
	if (pos1 == std::string::npos) {
		result.push_back(std::string(sv.substr(0, pos1)));
		return result;
	}
	auto pos2 = sv.find_first_of(pos1, ' ');
	std::string intStr = std::string(sv.substr(pos1 + 1, pos2));
	auto res = tryToInt(intStr);
	if (res.has_value())
		result.push_back(res.value());
	else
		result.push_back("");
	return result;
}

void goMultiply(int count) {
	CounterManager counterManager;
	for (int i = 0; i < count; ++i)
		counterManager.createCounter(i);

	std::string cmd;
	while (std::cin >> cmd) {

			
	
	}
	

}

using Variant = std::variant<std::string, int>;

int main() {
	
	std::string str = "qwerty";
	std::string_view sv(str);
	std::vector<Variant> res = parceCmd(sv).value();
	for (const auto& i : res)
		std::cout << std::visit([](auto&& arg) { return arg; }, i) << std::endl;


	return 0;

}