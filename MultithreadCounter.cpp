#include "MultithreadCounter.h"

BackgroundCounter::BackgroundCounter() : m_target(defaultTarget), m_value(0), m_paused(false), m_stopped(false) {}

BackgroundCounter::BackgroundCounter(int target) : m_target(target), m_value(0), m_paused(false), m_stopped(false) {}

BackgroundCounter::~BackgroundCounter() {
	stop();
	if (workerThread.joinable())
		workerThread.join();		//you don`t need to specify join  (std::jthread C++20)
}

void BackgroundCounter::start() {
	if (workerThread.joinable())
		return;
	workerThread = std::jthread([this](std::stop_token st) {worker(st);});
}

void BackgroundCounter::pause() {
	std::lock_guard<std::mutex> lock(mutex);
	m_paused = true;
	std::cout << "Paused" << std::endl;
}

void BackgroundCounter::resume() {
	{
		std::lock_guard<std::mutex> lock(mutex);
		m_paused = false;
	}
	cv.notify_one();
	std::cout << "Resumed" << std::endl;
}

void BackgroundCounter::stop() {
	{
		std::lock_guard<std::mutex> lock(mutex);
		m_paused = false;
		m_stopped = true;
	}
	cv.notify_all();
	if (workerThread.joinable()) {
		workerThread.request_stop();
		workerThread.join();
	}
}

int BackgroundCounter::getValue() const {
	std::lock_guard<std::mutex> lock(mutex);
	return m_value;
}

void BackgroundCounter::worker(std::stop_token st) {
	while (!st.stop_requested()) {
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, [this, &st]() {return !m_paused || m_stopped || st.stop_requested(); });

		if (m_stopped) {
			std::cout << "Thread stppped" << std::endl;
			break;
		}

		if (m_value >= m_target) {
			std::cout << "Target reached " << m_target << std::endl;
			break;
		}
		
		++m_value;
		int currentValue = m_value;
		lock.unlock();

		std::cout << "Counter: " << currentValue << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}

}

Counter::Counter(uint64_t id) :id(id), BackgroundCounter(){};

Counter::Counter(uint64_t id, int target) :id(id), BackgroundCounter(target) {};

Counter::~Counter() {};

void Counter::worker(std::stop_token st) {
	while (!st.stop_requested()) {
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, [this, &st]() { return !m_paused || m_stopped || st.stop_requested(); });

		if (m_stopped) {
			std::cout << "Thread " << id << " is stppped" << std::endl;
			break;
		}

		if (m_value >= m_target) {
			std::cout << "In thread" << id << " Target reached " << m_target << std::endl;
			break;
		}

		++m_value;
		int currentValue = m_value;
		lock.unlock();

		std::cout << "Counter " << id << ": " << currentValue << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}

void Counter::start() {
	if (workerThread.joinable())
		return;
	workerThread = std::jthread([this](std::stop_token st) {worker(st); });
}

void Counter::pause() {
	std::lock_guard<std::mutex> lock(mutex);
	m_paused = true;
	std::cout << "Counter " << id << " stopped" << std::endl;
}

void Counter::resume() {
	{
		std::lock_guard<std::mutex> lock(mutex);
		m_paused = false;
	}
	cv.notify_one();
	std::cout << "Counter " << id << " resume" << std::endl;
}

void Counter::stop() {
	BackgroundCounter::stop();
}
	
uint64_t Counter::getId() const { return id; }



CounterManager::CounterManager() {};

CounterManager::~CounterManager() {};

Counter* CounterManager::findCounter(uint64_t id) {
	std::lock_guard<std::mutex> lock(mutex);
	return counters.contains(id) ? counters[id].get() : nullptr;		//return not owning pointer
}

void CounterManager::createCounter(uint64_t id, int target) {
	std::lock_guard<std::mutex> lock(mutex);
	if (counters.contains(id))
		return;
	counters[id] = std::make_unique<Counter>(id, target);
}

void CounterManager::pauseCounter(uint64_t id) {
	auto pCounter = findCounter(id);
	if (pCounter) pCounter->pause();
}

void CounterManager::resumeCounter(uint64_t id) {
	auto pCounter = findCounter(id);
	if (pCounter) pCounter->resume();
}

void CounterManager::stopCounter(uint64_t id) {
	auto pCounter = findCounter(id);
	if (pCounter) pCounter->stop();
}
void CounterManager::stopAllCounters() {
	std::lock_guard<std::mutex> lock(mutex);
	for (auto& [id, counter] : counters)
		counter->stop();
	counters.clear();
}

void CounterManager::printStatusCounters() const {
	std::lock_guard<std::mutex> lock(mutex);
	for (const auto& [id, counter] : counters)
		std::cout << "Counter " << id << ": " << counter->getValue() << std::endl;
}