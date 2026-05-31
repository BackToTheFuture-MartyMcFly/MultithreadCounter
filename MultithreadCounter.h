#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <unordered_map>
#include <memory>

static int defaultTarget = INT_MAX;

class BackgroundCounter {
protected:
	std::jthread workerThread;
	mutable std::mutex mutex;
	std::condition_variable cv;
	bool m_paused = false;
	int m_value;
	const int m_target;
	std::atomic<bool> m_stopped{ false };

	virtual void worker(std::stop_token st);

public:
	explicit BackgroundCounter();
	explicit BackgroundCounter(int target);
	virtual ~BackgroundCounter();

	virtual void start();
	virtual void pause();
	virtual void resume();
	virtual void stop();
	int getValue() const;

};

class Counter : public BackgroundCounter {
private:
	uint64_t id;

	void worker(std::stop_token st) override;

public:
	explicit Counter(uint64_t id);
	explicit Counter(uint64_t id, int target);
	~Counter() override;

	void start() override;
	void pause() override;
	void resume() override;
	void stop() override;
	uint64_t getId() const;

};

class CounterManager {
private:
	std::unordered_map<int, std::unique_ptr<Counter>> counters;
	mutable std::mutex mutex;

	Counter* findCounter(uint64_t id);

public:
	CounterManager();
	~CounterManager();

	void createCounter(uint64_t id, int target = defaultTarget);
	void startCounter(uint64_t id);
	void pauseCounter(uint64_t id);
	void resumeCounter(uint64_t id);
	void stopCounter(uint64_t id);
	void stopAllCounters();
	void printStatusCounters() const;


};

